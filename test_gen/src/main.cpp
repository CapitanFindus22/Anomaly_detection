#include "main.hpp"
#include "CSVFile.hpp"

/*
    This program read data from a csv file, create a stream for each column
    and send one row of values from the file to its associated stream
*/
int main()
{

#if (DEBUG > 0)
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    setvbuf(stderr, (char *)NULL, _IONBF, 0);
#endif

    // Connect to the DB
    Con2DB db1(IP, PORT_DB, USERNAME, PASSWORD, DB_NAME);

    // Connect to Redis
    redisContext *c = redisConnect(IP, PORT);

    // Choose the csv file
    std::string CSVName = ChooseFile();
    CSVFile f(CSVName);

    // Each stream will be called STREAMnumber
    // Example: the first column will be attached to STREAM1
    std::string baseName = "STREAM";
    std::string names[f.num_columns];
    size_t i;

    // Array that will contain a row of values
    float values[f.num_columns];

    // Create log for this session and get the id
    log2db(std::ref(db1), f.num_columns, CSVName);
    int id = logfromdb(std::ref(db1), f.getName());

    // Clean the Stream, INFOSTREAM is used to exchange informations
    RedisCommand(c, "XTRIM INFOSTREAM 0");

    // Create the Stream names and clean them
    for (i = 0; i < f.num_columns; i++)
    {
        names[i] = baseName + std::to_string(i);
        RedisCommand(c, "XTRIM %s 0", names[i].c_str());
    }

    // Send to mean and covariance infos
    SendMessage(c, (float)f.num_columns, "INFOSTREAM");
    SendMessage(c, (float)id, "INFOSTREAM");

    // Read a line from the file and send it through the streams
    while (1)
    {

        // Convert the row to floats
        String2FloatArray(f.getline(), f.delimiter, values);

        for (i = 0; i < f.num_columns; i++)
        {
            SendMessage(c, values[i], names[i]);
        }

        sleep(1);
    }

    // Close the connection
    redisFree(c);

    return 0;
}