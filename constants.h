#define TZONE                 -5 // Eastern Standard Time
#define SERIALTXRATE          9600 // Baud Rate

#define MININCHES             2 //inches
#define MAXINCHES             20 //inches
#define SAMPLECOUNT           1 //Samples to aggregate before confirming state change

#define MAXTIMEBTNSAMPLES     300000 //
#define MAXTSDIFF             10000000 // 10 SECS
#define MAXULTRARANGE         10000
#define CLOUDUPDATEINTERVAL   90000000 //15 mins

// Required pauses for the sensor
#define TRIGGERINTERVAL       100 //Time between successive triggers in milliseconds. Required for proper function of Sensor
#define TRIGGERPAUSE          10 //Time between trigger push and release in microseconds. Required for proper function of Sensor

// Webhooks
#define WEBHOOKID1            "xing1" //left crossing webhook
#define WEBHOOKID2            "xing2" //right crossing webhook

// Debug levels on (1) or off (0)
#define DBGLVL0               1 // Human action info
#define DBGLVL1               1 // System action info
#define DBGLVL2               1 // State change info
#define DBGLVL3               1 // variable or branch change info
