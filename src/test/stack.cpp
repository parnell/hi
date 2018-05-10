//#include <iostream>
//#include <boost/mpi.hpp>
//#include <numeric>
//#include <thread>
//#include <set>
//#include <list>
//#include <boost/serialization/export.hpp>
//#include "../main/utils/Timer.hpp"
//#include "../main/utils/stringutils.hpp"
//#include "../main/utils/vecutil.hpp"
//
//namespace mpi = boost::mpi;
//
//enum TagType {
//    QUIT, WORK, WORK_STATUS
//};
//
//class MultWorkItem;
//
//class WorkItem {}; /// Base class
//
//class MultWorkItem : WorkItem{
//    std::vector<int> v;
//    friend class boost::serialization::access;
//    template<class Archive>
//    void serialize(Archive & ar, const unsigned int version)
//    {
//        ar & mysize;
//        ar & v;
//    }
//
//public:
//    explicit MultWorkItem() = default;
//    unsigned long mysize;
//    explicit MultWorkItem(size_t size) : mysize(size){}
//
//    virtual std::list<MultWorkItem*> work() {
//        std::cout << "Doing work" << std::endl;
//        v.resize(mysize);
//        std::iota(v.begin(), v.end(), 0);
//
//        size_t m = 0;
//        for (auto it=v.begin(); it<v.end();++it){
//            m += (*it)*(*it)*(*it);
//        }
//        std::cout << "final " << v.size() << "  " << mysize << "  " << m  << std::endl;
//        return std::list<MultWorkItem*> NO_WORK(0);
//    };
//};
//
//class Secondary {
//    mpi::communicator world;
//
//    volatile bool quit;
//    std::list<WorkItem*> queue;
//    std::list<mpi::request> sends;
//    int rank;
//
//    void resolveSends(bool force=false){
//        if (!sends.empty()){
//            sends.remove_if([](mpi::request req){ return req.test(); });
//        }
//    }
//public:
//    Secondary(int rank) : rank(rank) {
//        quit = false;
//    }
//
//
//    ~Secondary() {}
//
//
//    void dowork(){
//        if (!queue.empty()){ /// Do work
//            WorkItem* pwi = queue.front();
//            queue.pop_front();
//            std::list<WorkItem*> l = pwi->work();
//            for (auto& i : l){
//                queue.push_back(i);
//            }
//
//            sends.push_back(world.isend(0, TagType::WORK_STATUS, queue.size()));
//            checkQuit(); /// make sure we respond to quits
//        }
//        resolveSends();
//    }
//
//    void checkMessages() {
//        while (world.iprobe(0, TagType::WORK)) {
//            std::vector<WorkItem*> pwi;
//            world.recv(0, TagType::WORK, pwi);
//            std::cout << "    Slave " << world.rank() << "  found (" << pwi.size() << " ) " << std::endl;
//            for (auto& i : pwi){
//                queue.push_back(i);
//            }
//        }
//    }
//
//    bool checkQuit() {
//        if (world.iprobe(0, TagType::QUIT)) {
//            std::cout << "    Slave " << world.rank() << "  found QUIT  " << std::endl;
//            world.recv(0, TagType::QUIT);
//            quit = true;
//            return true;
//        }
//        return false;
//    }
//
//    void Secondary::run() {
//        /// take everything from the queue and do the work
//        while (!quit) {
//            checkMessages(); /// check for more workitems
//
//            dowork(); /// check for work
//
//            if (checkQuit()){ /// check for quit
//                resolveSends();
//                break;}
//            /// for some reason going through this loop too fast causes mpi boost to crash using iprobe
//            /// Yield is insufficient
//            std::this_thread::sleep_for(std::chrono::microseconds(1000));
//            std::this_thread::yield();
//        }
//    }
//
//
//};
//
//class MultJob;
//class Primary {
//    mpi::communicator world;
//    std::list<MultWorkItem*> myitems;
//    std::vector<size_t> workRemaining;
//    std::list<mpi::request> sends;
//    std::list<mpi::request> recvs;
//
//    void resolveSends(){
//        if (!sends.empty()) {
//            sends.remove_if([](mpi::request req) { return req.test(); });
//        }
//    }
//
//
//    bool isComplete() {
//        for (auto& i : workRemaining){
//            if (i > 0){
//                return false; }
//        }
//        return true;
//    }
//
//    public:
//
//    static void runWorker(){
//        Secondary worker(0);
//        worker.run();
//    }
//
//    void runjobs(){
//        std::vector<int> workerIds(world.size());
//        std::iota(workerIds.begin(), workerIds.end(), 0);
//        const int nworkers = static_cast<int>(workerIds.size());
//
//        std::thread workerThread = std::thread(runWorker);
//        std::list<MultWorkItem*> allitems = myitems;
//        int sendto;
//        while (true) {
//            /// Send all of items
//            if (!allitems.empty()){
//                std::vector<MultWorkItem*> v{ std::begin(allitems), std::end(allitems) };
//                for (int i=0, sendto=1; i< nworkers;++i, ++sendto){
//                    std::vector<MultWorkItem *> send(1);
//                    send[0] = v[i];
//                    std::cout << " >>> " << workerIds[sendto % nworkers] << " " << send.size() << " " << allitems.size() << std::endl;
//                    sends.push_back(world.isend(workerIds[sendto % nworkers], TagType::WORK, send));
//                    workRemaining[sendto] += send.size();
//                }
//            }
//
//            std::this_thread::yield();
//            std::for_each(allitems.begin(), allitems.end(), vecutil::DeleteVector<MultWorkItem*>());
//            allitems.clear();
//
//            /// Check for done items
//            for (auto& i : workerIds) {
//                while (world.iprobe(i, TagType::WORK_STATUS)) {
//                    unsigned int r;
//                    mpi::request req = world.irecv(i, TagType::WORK_STATUS, r);
//                    workRemaining[i] = r;
//                    std::cout <<  "                 <<< Slave " <<i << "  remaining=" << r << "  complete=" << isComplete() << std::endl;
//                }
//            }
//
//            if (jh.isComplete()){
//                std::cout << "====\n >>> complete " << std::endl;
//                break;
//            }
//            std::this_thread::sleep_for(std::chrono::milliseconds(2));
//            std::this_thread::yield();
//            resolveSends();
//        }
//
//        std::cout << "======================  sending quit " << std::endl;
//        Timer t("quittime");
////    std::vector< std::vector<T> > all_numbers;
//        mpi::request reqs[nworkers];
//        int n=0;
//        for (auto& i : workerIds) {
//            reqs[n++] = world.isend(i, TagType::QUIT);
//        }
//        mpi::wait_all(reqs, reqs + nworkers);
//        std::cout << "======================  gathering " << std::endl;
//        workerThread.join();
//        std::cout << "======================  quitting " << std::endl;
//    }
//
//    void addJob(MultJob* pjob);
//};
//
//
//BOOST_CLASS_EXPORT_GUID(MultWorkItem, "MultWorkItem");
//
//class MultJob {
//public:
//    std::list<MultWorkItem*> unfinished;
//
//    MultJob(size_t size, int splits){
//        for (size_t i = 0; i < splits; ++i) {
//            unfinished.push_back(new MultWorkItem(size));
//        }
//    }
//};
//
//int main(int argc, char** argv) {
//    unsigned int nthreads = std::thread::hardware_concurrency();
//    std::cout << "Starting argc=" << argc << std::endl;
//    for (int k = 0; k < argc; ++k) {
//        std::cout << argv[k] << " ";
//    }
//    std::cout << std::endl;
//
//    mpi::environment env;
//    mpi::communicator world;
//    const int wsize = world.size();
//    std::cout << "rank=" << world.rank() << "\tsize=" << wsize << "  nthreads=" << nthreads << std::endl;
//    /// Build Job
//    Timer t(sutil::sformat("%d", world.rank()));
//
//    if (world.rank() == 0) {
//        MultJob* pj = new MultJob(10000000, 4);
//        /// create primary
//        Primary m;
//        m.addJob(pj);
//        m.runjobs();
//    } else {
//        /// Create secondary
//        Secondary m(world.rank());
//        m.run();
//    }
//    return 0;
//}
