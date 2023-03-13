# to do
* only try to grab if time is out
* poll lease at a frequency or use watch
* get lease for first time
* continually refresh lease and grab if expired
* spawn 3 and kill them in random orders to ensure it works as expected
* create a leader elector sidecar container
    * accepts tcp conns
    * on conn you add the lease you want to be active for
    * on active you are infromed
    * on passive you are informed
* create a service registrar ambassador container
    * web server
    * you register and derigster for services
    * in turn it adds the correct labels to the pod for the service
