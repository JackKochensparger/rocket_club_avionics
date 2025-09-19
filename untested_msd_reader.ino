#include <SPI.h>
#include <SD.h>

// SD Card pin configuration
const int chipSelect = 25;  // Your SD card CS pin

File myFile;
String inputString = "";
boolean stringComplete = false;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect (needed for some boards)
  }
  
  Serial.println("=== SD Card Reader for SparkFun NINA B306 ===");
  Serial.println("Initializing SD card...");
  
  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("ERROR: SD card initialization failed!");
    Serial.println("Check connections and make sure SD card is inserted.");
    return;
  }
  
  Serial.println("SD card initialized successfully!");
  
  // Print card info
  printCardInfo();
  
  // List all files
  Serial.println("\n=== Files on SD Card ===");
  listFiles(SD.open("/"), 0);
  
  // Print available commands
  printCommands();
}

void loop() {
  // Check for serial input
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  
  // Process commands
  if (stringComplete) {
    inputString.trim();
    processCommand(inputString);
    inputString = "";
    stringComplete = false;
  }
}

void printCommands() {
  Serial.println("\n=== Available Commands ===");
  Serial.println("list - List all files on SD card");
  Serial.println("read <filename> - Read and display file contents");
  Serial.println("download <filename> - Download file (optimized for binary transfer)");
  Serial.println("info - Show SD card information");
  Serial.println("help - Show this help menu");
  Serial.println("===========================");
  Serial.println("Enter command:");
}

void processCommand(String command) {
  command.toLowerCase();
  
  if (command == "list") {
    Serial.println("\n=== Files on SD Card ===");
    listFiles(SD.open("/"), 0);
  }
  else if (command.startsWith("read ")) {
    String filename = command.substring(5);
    readFile(filename);
  }
  else if (command.startsWith("download ")) {
    String filename = command.substring(9);
    downloadFile(filename);
  }
  else if (command == "info") {
    printCardInfo();
  }
  else if (command == "help") {
    printCommands();
  }
  else {
    Serial.println("Unknown command. Type 'help' for available commands.");
  }
  
  Serial.println("\nEnter command:");
}

void listFiles(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break; // No more files
    }
    
    // Print tabs for subdirectories
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      // Recursively list subdirectory contents
      listFiles(entry, numTabs + 1);
    } else {
      // Print file size
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      Serial.println(" bytes");
    }
    entry.close();
  }
}

void readFile(String filename) {
  Serial.println("\n=== Reading File: " + filename + " ===");
  
  myFile = SD.open(filename);
  if (myFile) {
    Serial.println("File size: " + String(myFile.size()) + " bytes");
    Serial.println("--- File Contents ---");
    
    // Read and print file contents
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    
    myFile.close();
    Serial.println("\n--- End of File ---");
  } else {
    Serial.println("ERROR: Could not open file '" + filename + "'");
  }
}

void downloadFile(String filename) {
  Serial.println("\n=== Downloading File: " + filename + " ===");
  
  myFile = SD.open(filename);
  if (myFile) {
    unsigned long fileSize = myFile.size();
    
    // Send download header
    Serial.println("DOWNLOAD_START");
    Serial.println("FILENAME:" + filename);
    Serial.println("SIZE:" + String(fileSize));
    Serial.println("DATA_START");
    
    // Send file data in chunks
    byte buffer[512];
    unsigned long bytesRead = 0;
    
    while (myFile.available()) {
      int chunkSize = myFile.read(buffer, sizeof(buffer));
      Serial.write(buffer, chunkSize);
      bytesRead += chunkSize;
    }
    
    myFile.close();
    Serial.println("\nDATA_END");
    Serial.println("DOWNLOAD_COMPLETE");
    Serial.println("Transferred " + String(bytesRead) + " bytes");
  } else {
    Serial.println("ERROR: Could not open file '" + filename + "'");
  }
}

void printCardInfo() {
  Serial.println("\n=== SD Card Information ===");
  Serial.println("SD card is accessible and working!");
  Serial.println("Use 'list' command to see files and their sizes.");
  Serial.println("===========================");
}