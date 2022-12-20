package mosso;

import mosso.algorithm.*;

import java.io.IOException;
import java.util.Date;

public class Run {

    private SupernodeHelper module;

    public static void main(String[] args) throws IOException {
        int interval=100000;
        Date today = new Date();
        System.out.println(today);
        String inputPath=" ", outputPath=" ", sumMode=" ";


        int now=0;
        while(now<args.length){
            if(args[now++].equals("-dataset")){
                inputPath=args[now++];
            }
            if(args[now++].equals("-result")){
                outputPath=args[now++];
            }
            if(args[now++].equals("-mode")){
                sumMode=args[now++];
            }
            if(args[now++].equals("-interval")){
                interval=Integer.parseInt(args[now++]);
             }
        }
        System.out.println("input_path: " + inputPath);
        System.out.println("output_path: " + outputPath);
        System.out.println("summarization_mode: " + sumMode);
        System.out.println("interval: " + interval);
        System.out.println();

        final SupernodeHelper module;

        final int probability = 3;
        final int n_samples = 1200;

        if (sumMode.compareTo("mosso") == 0) {
            System.out.println("escape probability : " + probability + ", n_samples : " + n_samples + ", TT : " + interval);
            module = new MoSSo(false, probability, n_samples, interval);
        } else if (sumMode.compareTo("simple") == 0) {
            System.out.println("escape probability : " + probability + ", n_samples : " + n_samples + ", TT : " + interval);
            module = new MoSSoSimple(false, probability, n_samples, interval);
        } else if (sumMode.compareTo("mcmc") == 0) {
            System.out.println("interval : " + interval);
            module = new MoSSoMCMC(false, interval);
        } else if(sumMode.compareTo("sgreedy") == 0) {
            System.out.println("interval : " + interval);
            module = new MoSSoGreedy(false, interval);
        } else {
            System.out.println("Invalid command.");
            return;
        }
        int edgeCount = Common.execute(module, inputPath, " ",interval, outputPath);
    }
}
