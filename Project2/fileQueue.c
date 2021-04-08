// how many threads are currently putting into dir q
// last thread waiting to deq, at that 
//point instead of waiting we need to close dir q, 
//wake up other threads and let them terminate