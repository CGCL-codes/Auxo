package mosso;

import it.unimi.dsi.fastutil.ints.IntArrayList;
import it.unimi.dsi.fastutil.ints.IntRBTreeSet;
import it.unimi.dsi.fastutil.objects.ObjectArrayList;
import java.util.Random;

import java.io.*;
import java.util.Arrays;
import java.util.List;
import java.util.ListIterator;
import com.carrotsearch.sizeof.*;

public class Common {
    private static final int sampleSize = 1000;
    private static final int[] from=new int[sampleSize];
    private static final int[] to=new int[sampleSize];
    private static int nowPos=0;
    private static final Random r = new Random();

    public Common(){}

    private static int[] parseEdge(String line, String delim){
        String[] tokens = line.split(delim);
        int useless;
        try {
            int src = Integer.valueOf(tokens[0]);
            int dst = Integer.valueOf(tokens[1]);
            int add = Integer.valueOf(tokens[2]);
            useless = Integer.valueOf(tokens[3]);
            return new int[]{src, dst, add, useless};
        }catch(Exception e){
            return null;
        }
    }

    public static int execute(final SupernodeHelper module, final String inputPath, final String delim, int interval, String outputPath) throws IOException{
        int count = 0;
        long start = System.currentTimeMillis();
        BufferedReader br = new BufferedReader(new FileReader(inputPath));
        long cnt=0;

        BufferedWriter bw = new BufferedWriter(new FileWriter(outputPath));


        while(true){
            cnt+=1;
            final String line = br.readLine();
            if(line == null) break;
            final int[] edge = parseEdge(line, delim);
            if(edge == null) break;
            count += edge[2];
            //System.out.println(edge[0]+"  "+edge[1]+"  "+edge[2]+"  "+edge[3]);
            if(edge[2] > 0) module.processAddition(edge[0], edge[1]);
            else module.processDeletion(edge[0], edge[1]);
            //System.out.println(edge[0]);
            if(cnt%interval==0){
                long memoryCost = RamUsageEstimator.sizeOf(module);
                bw.write(cnt+":");
                bw.write(" memory cost: " + memoryCost/(1048576.0)+"M ");
                bw.write("  update time : " + (double)(System.currentTimeMillis() - start)*1000 / interval+"us\n");
                bw.flush();
//                {
//                    long start1 = System.currentTimeMillis();
//                    for(int i=0;i<100;i++){
//                        int r1 = r.nextInt(sampleSize-1);
//                        module.edgeExist(from[r1],to[r1]);
//                    }
//                    System.out.print("  edge query time : " + (double)(System.currentTimeMillis() - start1)*1000 / 100+"us");
//
//                    start1 = System.currentTimeMillis();
//                    for(int i=0;i<100;i++){
//                        int r1 = r.nextInt(sampleSize-1);
//                        module.nodeDegree(from[r1]);
//                    }
//                    System.out.println("  node degree query time : " + (double)(System.currentTimeMillis() - start1)*1000 / 100+"us");
//                }
                start = System.currentTimeMillis();
            }
            from[nowPos]=edge[0];
            to[nowPos++]=edge[1];
            nowPos%=sampleSize;
        }
        module.processBatch();

        br.close();
        bw.close();
        long end = System.currentTimeMillis();
        System.out.println("Execution time: " + (end - start) / 1000.0 + "s.");
        return count;
    }

    public static void writeOutputs(final SupernodeHelper module, final String outputPath, final int edgeCount) throws IOException{
        /*
        Output:
        line 1: |V|, |E|
        line 2 ~ |V|+1: (real_index, membership(block) info)
        line |V|+2 ~ |V|+|P|+1: superedges in P
        line |V|+|P|+2: -1 -1
        line |V|+|P|+3 ~ |V|+|P|+|Cp|+2: edges in Cp
        line |V|+|P|+|Cp|+3: -1 -1
        line |V|+|P|+|Cp|+4 ~ |V|+|P|+|Cp|+|Cm|+3: edges in Cm
        line |V|+|P|+|Cp|+|Cm|+4: -1 -1
        */
        BufferedWriter bw = new BufferedWriter(new FileWriter(outputPath));
        Tuple<IntArrayList, AdjacencyList[], IntArrayList> compressedResult = module.getCompressedResult();
        int n = compressedResult.third.size();
        int[] summaryCount = {0, 0, 0};
        List<String> listNames = Arrays.asList("P", "C_plus", "C_minus");
        ListIterator<String> it = listNames.listIterator();
        IntArrayList realIdxs = compressedResult.third;

        // line 1
        bw.write(n + "\t" + edgeCount + "\n");

        // line 2 ~ |V|+1
        for(int i=0;i<n;i++){
            bw.write(realIdxs.getInt(i) + "\t" + compressedResult.first.getInt(i) + "\n");
        }

        while(it.hasNext()){
            int idx = it.nextIndex();
            AdjacencyList target = compressedResult.second[idx];
            it.next();
            for(int i=0;i<n;i++){
                for(int v: target.getNeighbors(i)){
                    if(i <= v){
                        bw.write(i + "\t" + v + "\n");
                        summaryCount[idx]++;
                    }
                }
            }
            bw.write("-1\t-1\n");
        }
        System.out.println("SUMMARY:");
        for(int i=0;i<3;i++){
            System.out.println("|" + listNames.get(i) + "|: " + summaryCount[i]);
        }
        int totalCount = (summaryCount[0] + summaryCount[1] + summaryCount[2]);
        System.out.println("total: " + totalCount);
        System.out.println("compression_rate: " + totalCount / (double) edgeCount);
        System.out.println();
        bw.close();
    }
}
