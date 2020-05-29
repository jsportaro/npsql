* Relational Database D&I!
    * Introduction   
        * Book Overview and Scope (basically this in prose)
        * Assumption - x86, Linux (maybe bsd)
    * Part 1 - Slow and Persistent
        * Introduction - Here I plan on building the database from the files to the network access.  A CLI client will be provided for "free" at the end to test. 
        * Storage Engine
            * Files
                * Using the OS FS vs. Rolling your own
                * Dealing with endiannes
                    * Big vs. Little and why we favor little
            * File Types
                * Data File
                * Log File
            * Buffer Manager
            * Log Manager
            * Records
            * Managing Metadata
    * Query Engine
        * Scans
        * Plans
        * Parsing SQL
        * Planner
    * Networking 
        * Protocol Overview
	  
    * Part 2 - Getting data faster

