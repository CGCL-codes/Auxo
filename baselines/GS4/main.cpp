#include "gs4.h"
#include <math.h>
using namespace std;

int main(int argc, char* argv[]) {
    double tmp=acos(24.0/27.0);
    double res=270*270*tmp-240*sqrt(270*270-240*240);
    cout<<res<<endl;
    return 0;
//    heap<pqnode<int,double>> pq;
//    for(int i=0;i<5;i++){
//        for(int j=0;j<5;j++){
//            pq.push(pqnode<int,double>(i,j,i+j));
//        }
//    }
//    for(int i=0;i<2;i++){
//        pq.del(pqnode<int,double>(i,i,-1));
//    }
//    while(!pq.isEmpty()){
//        cout<<pq.top().val1<<" "<<pq.top().val2<<endl;
//        pq.pop();
//    }
//    return 0;

    string file_name="test.txt",result_dir="res.txt";
    int interval=100,diff=300,cnt=10;
    // command line parameters
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-dataset") == 0) {
            file_name = argv[++i];
        }
        if (strcmp(argv[i], "-result") == 0) {
            result_dir = argv[++i];
        }
        if (strcmp(argv[i], "-interval") == 0) {
            interval = atoi(argv[++i]);;
        }
        if (strcmp(argv[i], "-diff") == 0) {
            diff = atoi(argv[++i]);
        }
        if (strcmp(argv[i], "-cnt") == 0) {
            cnt = atoi(argv[++i]);
        }
    }
    graph gra;
    cout<<"the parameters are as below:\ndataset: "<<file_name<<"\nresult_dir: "<<result_dir<<"\nsummarize_interval: "<<interval
    <<"\nsummarize_cnt: "<<cnt<<"\ndifference: "<<diff;

	ifstream ifs;
	ifs.open(file_name);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << file_name << endl;
		return -1;
	}
	cout << "\nInserting..." << endl;
	ll s,d,w,t;

	struct timeval step1;
	struct timeval step2;
	double alltime;

	ofstream out1(result_dir);

	ll iteration=0;

	vector<ll> target;
	int base=interval;
	int pos=0;
	while(base<1e9){
        target.push_back(base);
        target.push_back(4*base);
        target.push_back(7*base);
        base*=10;
	}

    while (!ifs.eof()) {
        iteration++;
        ifs >> s >> d >> w >> t;
        gra.insert_edge(to_string(s),to_string(d));

        if (iteration==target[pos])
        {
            pos++;
            cout<<"\nsummarizing "<<iteration<<" edges "<<" to"<<(iteration*cnt/interval)<<" nodes";
            gettimeofday(&step1, NULL);
            gra.summarize(iteration*cnt/interval);
            gettimeofday(&step2, NULL);
            alltime = (step2.tv_sec-step1.tv_sec)*1000000.0 +
                      (step2.tv_usec-step1.tv_usec);
            out1<<iteration<<" ";
            out1<<"memory cost: "<<physical_memory_used_by_process()/1024.0<<"M";
            out1<<" update time: "<<alltime<<" us\n";
            out1.flush();
            gra.print(to_string(iteration)+"_"+result_dir);
            gettimeofday(&step1, NULL);
        }
    }

    ifs.close();
    out1.close();
    return 0;
}
