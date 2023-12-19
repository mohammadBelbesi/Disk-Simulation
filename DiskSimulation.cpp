//mohammad belbesi
//DiskSimulation
#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <cstring>
#include <cmath>
#include <fcntl.h>
#include <string>
#include <sstream>

using namespace std;

#define DISK_SIZE 512

// Function to convert decimal to binary char
// char decToBinary(int n)
char decToBinary(int n)
{
    return static_cast<char>(n);
}

// Function to convert binary char to decimal
// int binaryToDec(char c)
int charToDec(unsigned char c)
{
    return static_cast<int>(c);
}

// #define SYS_CALL
/** ============================================================================ **/
class fsInode
{
    int fileSize;
    int block_in_use;

    int directBlock1;
    int directBlock2;
    int directBlock3;

    int singleInDirect;
    int doubleInDirect;
    int block_size;

public:
    fsInode(int _block_size)
    {
        fileSize = 0;
        block_in_use = 0;
        block_size = _block_size;
        directBlock1 = -1;
        directBlock2 = -1;
        directBlock3 = -1;
        singleInDirect = -1;
        doubleInDirect = -1;
    }

    /** setters and getters **/

    void incrementFileSize(int to_add)
    {
        this->fileSize += to_add;
    }

    void setFileSize(int newFileSize)
    {
        this->fileSize = newFileSize;
    }

    int getFileSize()
    {
        return this->fileSize;
    }

    void incrementBlockInUse()
    {
        this->block_in_use++;
    }

    int getBlockInUse()
    {
        return this->block_in_use;
    }

    void setDirectBlock1(int setDB1)
    {
        this->directBlock1 = setDB1;
    }

    int getDirectBlock1() const
    {
        return this->directBlock1;
    }

    void setDirectBlock2(int setDB2)
    {
        this->directBlock2 = setDB2;
    }

    int getDirectBlock2() const
    {
        return this->directBlock2;
    }

    void setDirectBlock3(int setDB3)
    {
        this->directBlock3 = setDB3;
    }

    int getDirectBlock3() const
    {
        return this->directBlock3;
    }

    void setSingleInDirect(int setSID)
    {
        this->singleInDirect = setSID;
    }

    int getSingleInDirect() const
    {
        return this->singleInDirect;
    }

    void setDoubleInDirect(int setDID)
    {
        this->doubleInDirect = setDID;
    }

    int getDoubleInDirect() const
    {
        return this->doubleInDirect;
    }

};

/** ============================================================================ **/
class FileDescriptor
{
    pair<string, fsInode *> file;
    bool inUse; // true if the file is open and false if it closed
    bool deleted;

public:
    FileDescriptor(string FileName, fsInode *fsi)
    {
        file.first = FileName;
        file.second = fsi;
        inUse = true;
        deleted = false;
    }

    string getFileName()
    {
        return file.first;
    }

    void setFileName(const std::string &SetNewFileName)
    {
        if (!SetNewFileName.empty())
        {
            file.first = SetNewFileName;
        }
    }

    fsInode *getInode()
    {
        return file.second;
    }

    int GetFileSize()
    {
        return file.second->getFileSize();
    }

    bool isInUse()
    {
        return (this->inUse);
    }

    void setInUse(bool _inUse)
    {
        this->inUse = _inUse;
    }

    bool getDeletedStatus() const
    {
        return (this->deleted);
    }

    void setDeletedStatus(bool deletedStatus)
    {
        this->deleted = deletedStatus;
    }
};

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
/** ============================================================================ **/
class fsDisk
{
    FILE *sim_disk_fd;

    bool is_formated;

    // BitVector - "bit" (int) vector, indicate which block in the disk is free
    //              or not.  (i.e. if BitVector[0] == 1 , means that the
    //             first block is occupied.
    int BitVectorSize=0;
    int *BitVector={};
    int block_size=0;

    // Unix directories are lists of association structures,
    // each of which contains one filename and one inode number.
    map<string, fsInode *> MainDir;

    // OpenFileDescriptors --  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.
    vector<FileDescriptor> OpenFileDescriptors;

public:

    // ------------------------------------------------------------------------
    fsDisk() {
        sim_disk_fd = fopen( DISK_SIM_FILE , "r+" );
        assert(sim_disk_fd);
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd );
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);

    }

    // ------------------------------------------------------------------------
    void listAll() {
        int i = 0;
        for ( auto it = begin (OpenFileDescriptors); it != end (OpenFileDescriptors); ++it) {
            cout << "index: " << i << ": FileName: " << it->getFileName() << " , isInUse: "
                 << it->isInUse() << " file Size: " << it->GetFileSize() << endl;
            i++;
        }
        char bufy;
        cout << "Disk content: '" ;
        for (i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fread(  &bufy , 1 , 1, sim_disk_fd );
            cout << bufy;
        }
        cout << "'" << endl;


    }

    // ------------------------------------------------------------------------
    void fsFormat(int blockSize = 4)
    {
        if ((this->is_formated))
        { // check if the disk has been formated, if not return -1
            // Deallocate BitVector
            delete[] BitVector;
            // Delete all the fsInode pointers in MainDir
            for (auto &entry : MainDir)
            {
                delete entry.second;
            }
            MainDir.clear();

            // No need to delete anything from OpenFileDescriptors, because it contains
            // objects, not pointers. Also, the fsInode pointers in OpenFileDescriptors
            // are also stored in MainDir, and we've already deleted them.

            // Clear OpenFileDescriptors
            OpenFileDescriptors.clear();

            /********************************************************************************/
            // Fill a buffer with 512 null bytes
            char nullBuffer[DISK_SIZE];
            memset(nullBuffer, 0, sizeof(nullBuffer));

            // Go to the beginning of the file
            fseek(sim_disk_fd, 0, SEEK_SET);

            // Write the null bytes to the file
            fwrite(nullBuffer, 1, sizeof(nullBuffer), sim_disk_fd);

            // Flush any buffered output for the given output or update stream
            fflush(sim_disk_fd);

        }
        this->block_size = blockSize;

        /** initialize the bitVector**/
        this->BitVectorSize = DISK_SIZE / this->block_size;
        this->BitVector = new int[this->BitVectorSize];
        if (this->BitVector == nullptr)
        { // the code is reachable if there are not enough memory
            cerr << "allocating memory for bitVector is failed"<<endl;
            exit(EXIT_FAILURE);
        }
        // fill the bitVector with zeros
        std::fill(this->BitVector, this->BitVector + this->BitVectorSize, 0);
        /** end of initialize the bitVictor**/
        this->is_formated = true;
        cout << "FORMAT DISK: number of blocks: " << DISK_SIZE / blockSize << endl;
    }

    // ------------------------------------------------------------------------
    int CreateFile(string fileName)
    {
        if (!(this->is_formated))
        { // check if the disk has been formated, if not return -1
            cerr << "the disk has not formated!!"<<endl;
            return -1;
        }
        /**
         *  to create file we should check the following situations :
         *  1- check if the file is already exist by using find function and return -1 if its exist.
         *  2- if the file is not exist or if the mainDir is empty create fsInode and also update the openFileDescriptor and the mainDir
         **/

        /**1-**/
        map<string, fsInode *>::iterator itr;
        itr = this->MainDir.find(fileName);
        if (itr != this->MainDir.end())
        {
            cerr << "the file has been already created before!!"<<endl;
            return -1;
        }
        /**2-**/
        fsInode *newInode;
        try {
            newInode = new fsInode(this->block_size);
        } catch (std::bad_alloc&) {
            cerr << "create a new Inode fail" << endl;
            return -1;
        }
        /*if (newInode == nullptr)
        {
            cerr << "create a new Inode fail"<<endl;
            return -1;
        }*/
        FileDescriptor fd(fileName, newInode);
        fd.setInUse(true);
        fd.setDeletedStatus(false);
        this->MainDir[fileName] = newInode;
        this->OpenFileDescriptors.push_back(fd);
        return (int)this->OpenFileDescriptors.size() - 1; // the fd number in OpenFileDescriptors
    }

    // ------------------------------------------------------------------------
    /** the function will return the fd of the file with the given fileName after opening the file, if an error occurred the func will return a -1**/
    int OpenFile(string FileName)
    {
        if (!(this->is_formated))
        { // check if the disk has been formated, if not return -1
            cerr << "the disk has not formated!!"<<endl;
            return -1;
        }
        /**check if the file is not exist by using find function and return -1 if its not exist.**/
        map<string, fsInode *>::iterator itr;
        itr = this->MainDir.find(FileName);
        if (itr == this->MainDir.end())
        {
            cerr << "the file dose not exist!!" << endl;
            return -1;
        }
        for (int i = 0; i < (int)this->OpenFileDescriptors.size(); i++)
        {
            if (this->OpenFileDescriptors[i].getFileName() == FileName && !this->OpenFileDescriptors[i].isInUse() &&
                !this->OpenFileDescriptors[i].getDeletedStatus())
            {
                this->OpenFileDescriptors[i].setInUse(true);
                return i;
            }
        }
        cerr << "opening file error!!" << endl;
        return -1;
    }

    // ------------------------------------------------------------------------
    /** the function will return the name of the file with the given fd after closing the file, if an error occurred the func will return a "-1"**/
    string CloseFile(int fd)
    {
        if (!(this->is_formated))
        { // check if the disk has been formated, if not return -1
            cerr << "the disk has not formated!!"<<endl;
            return "-1";
        }
        if (fd >= 0 && fd <= (int)this->OpenFileDescriptors.size() - 1)
        { // check if the file is existing
            if (this->OpenFileDescriptors[fd].isInUse())
            { // check if the file is opened
                this->OpenFileDescriptors[fd].setInUse(false);
                return this->OpenFileDescriptors[fd].getFileName();
            }
        }
        cerr << "closing file error!!" << endl;
        return "-1";
    }
    // ------------------------------------------------------------------------
    /**  the function will return the number of the written bits after writing to the file with the specific fd ,if an error occurred the func will return a -1**/
    int WriteToFile(int fd, char *buf, int len)
    {
        int empty_blocks = this->emptyBlocksCounter();
        /**check the format of the disk and the file status**/

        if (!(this->is_formated))
        { // check if the disk has been formated, if not return -1
            cerr << "the disk has not formatted!!" << endl;
            return -1;
        }
        if (fd < 0 || fd > (int)this->OpenFileDescriptors.size() - 1)
        { // check if the file is not existing
            cerr << "the file is not existing!!" << endl;
            return -1;
        }

        if (this->OpenFileDescriptors[fd].getDeletedStatus())
        { // check if the file has been deleted
            cerr << "the file has been deleted!!" << endl;
            return -1;
        }

        if (!this->OpenFileDescriptors[fd].isInUse())
        { // check if the file is opened
            cerr << "the file is not opened!!" << endl;
            return -1;
        }
        if (empty_blocks == 0)
        {
            cerr << "disk is full can't write more files!!" << endl;
        }

        fsInode *fileToUse = this->OpenFileDescriptors[fd].getInode();
        int file_size = fileToUse->getFileSize();
        int block_needed = len / this->block_size;
        int freeSpaceInLastBlock = 0;

        /**Calculating Free Space in the Last Block**/

        if (this->OpenFileDescriptors[fd].GetFileSize() % this->block_size != 0)
        {
            freeSpaceInLastBlock = block_size - this->OpenFileDescriptors[fd].GetFileSize() % this->block_size;
        }

        int maxDirectBlocksSize = 3 * this->block_size;
        int maxSingleInDirectSize = maxDirectBlocksSize + this->block_size * this->block_size;
        int maxDoubleInDirectSize = maxSingleInDirectSize + this->block_size * this->block_size * this->block_size;
        int maxFileSize=maxDirectBlocksSize+maxSingleInDirectSize+maxDoubleInDirectSize;

        if(len>freeSpaceInLastBlock){
            if (len % this->block_size > 0)
            {
                block_needed += 1;
            }
        }

        int written_chars = 0;
        int current_block_index = 0;
        int remaning_chars = len;
        if (this->GetFileSize(fd) == 0 || (this->GetFileSize(fd) % block_size == 0 && freeSpaceInLastBlock == 0))
        {
            if (fileToUse->getFileSize() < maxDirectBlocksSize)
            {
                current_block_index = this->firstEmptyBlock();
                setNextAvailableBlockIndex(fileToUse, current_block_index);
                freeSpaceInLastBlock = block_size;
            }
            else if((fileToUse->getFileSize() <maxSingleInDirectSize || fileToUse->getFileSize() <maxDoubleInDirectSize)&&file_size< maxFileSize)
            {
                current_block_index = this->findEmptyBlockFromTheEnd();
                setNextAvailableBlockIndex(fileToUse, current_block_index);
                freeSpaceInLastBlock = block_size;
            }
            else{
                cerr<<"there are no more storage in the disk"<<endl;
            }
        }

        while (written_chars < len)
        {
            current_block_index = this->getCurrentPointerBlockIndex(fileToUse);
            int position = (this->block_size * current_block_index) + (this->block_size - freeSpaceInLastBlock);
            if (fseek(sim_disk_fd, position, SEEK_SET) != 0)
            {
                perror("error when seek a position for writing\n");
                return -1;
            }
            int chars_to_write = 0;

            if (remaning_chars <= freeSpaceInLastBlock)
            {
                chars_to_write = remaning_chars;
                freeSpaceInLastBlock -= remaning_chars;
                fileToUse->incrementFileSize(chars_to_write);
                if ((int)(fwrite(buf + written_chars, 1, chars_to_write, sim_disk_fd)) != chars_to_write)
                {
                    perror("error when write char for position\n");
                    return -1;
                }
            }
            else
            {
                chars_to_write = freeSpaceInLastBlock;
                freeSpaceInLastBlock = this->block_size;
                fileToUse->incrementFileSize(chars_to_write);
                if ((int)(fwrite(buf + written_chars, 1, chars_to_write, sim_disk_fd)) != chars_to_write)
                {
                    perror("error when write char for position\n");
                    return -1;
                }
                if (remaning_chars != block_size)
                {
                    if (fileToUse->getFileSize() < maxDirectBlocksSize)
                    {
                        current_block_index = this->firstEmptyBlock();
                    }
                    else if (fileToUse->getFileSize() < maxSingleInDirectSize || fileToUse->getFileSize() < maxDoubleInDirectSize)
                    {
                        current_block_index = this->findEmptyBlockFromTheEnd();
                    }
                    else
                    {
                        cerr << "Maximum file size reached!" << endl;
                        return -1;
                    }
                    setNextAvailableBlockIndex(fileToUse, current_block_index);
                }
            }

            written_chars += chars_to_write;
            remaning_chars -= chars_to_write;
        }

        return 1;
    }
    // ------------------------------------------------------------------------
    int DelFile(string FileName)
    {
        map<string, fsInode *>::iterator it;
        fsInode *fs = nullptr;
        it = this->MainDir.find(FileName);
        if (it == this->MainDir.end())
        {
            fprintf(stderr, "the file dose not exist\n");
            return -1;
        }
        int i;
        for (i = 0; i < (int)this->OpenFileDescriptors.size(); i++)
        {
            if (!this->OpenFileDescriptors[i].getDeletedStatus() && this->OpenFileDescriptors[i].getFileName() == FileName)
            {
                if (this->OpenFileDescriptors[i].isInUse())
                {
                    cerr << "you cannot delete an open file"<<endl;
                    return -1;
                }
                fs = this->OpenFileDescriptors[i].getInode();
                break;
            }
        }
        if (fs == nullptr)
        {
            cerr << "the file has been deleted already"<<endl;
            return -1;
        }
        this->BitVector[i]=0;
        fs->setFileSize(0);
        this->OpenFileDescriptors[i].setFileName(" ");
        delete fs;
        this->MainDir.erase(FileName);
        this->OpenFileDescriptors[i].setDeletedStatus(true);
        this->OpenFileDescriptors[i].setInUse(false);
        return i;
    }

    int ReadFromFile(int fd, char *buf, int len){
        // Initializing the buffer to zeros at the beginning.
        memset(buf, 0, len);

        if(len <= 0){
            cerr << "the num of chars to read should be more than 0" << endl;
            return -1;
        }

        if (!this->is_formated) {
            cerr << "the disk has not formatted!!" << endl;
            return -1;
        }

        if (fd < 0 || fd > (int)this->OpenFileDescriptors.size() - 1) {
            cerr << "the file is not existing!!" << endl;
            return -1;
        }

        if (this->OpenFileDescriptors[fd].getDeletedStatus()) {
            cerr << "the file has been deleted!!" << endl;
            return -1;
        }

        if (!this->OpenFileDescriptors[fd].isInUse()) {
            cerr << "the file is not opened!!" << endl;
            return -1;
        }

        fsInode *fileToUse = this->OpenFileDescriptors[fd].getInode();
        int file_size = fileToUse->getFileSize();
        int max_len = std::min(file_size, len);  // Ensuring we don't read more than the file's size or the requested len.

        int *blocks_indexes = new int[fileToUse->getBlockInUse()];
        int i = 0;

        // Direct blocks.
        if (fileToUse->getDirectBlock1() != -1) {
            blocks_indexes[i] = fileToUse->getDirectBlock1();
            i++;
        }
        if (fileToUse->getDirectBlock2() != -1) {
            blocks_indexes[i] = fileToUse->getDirectBlock2();
            i++;
        }
        if (fileToUse->getDirectBlock3() != -1) {
            blocks_indexes[i] = fileToUse->getDirectBlock3();
            i++;
        }

        // Single indirect block.
        if (fileToUse->getSingleInDirect() != -1) {
            char to_read = '\0';
            for (int j = 0; j < block_size && i < fileToUse->getBlockInUse(); j++) {
                fseek(sim_disk_fd, (fileToUse->getSingleInDirect() * block_size) + j, SEEK_SET);
                fread(&to_read, 1, 1, sim_disk_fd);
                int block_index = charToDec(to_read);
                if (block_index != -1) {
                    blocks_indexes[i] = block_index;
                    i++;
                }
            }
        }

        // Double indirect block.
        if (fileToUse->getDoubleInDirect() != -1)
        {
            char outerBlockIndexChar;
            for (int outerPosition = 0; outerPosition < block_size; outerPosition++) // Assuming block size is the maximum number of entries in outer block
            {
                fseek(sim_disk_fd, (fileToUse->getDoubleInDirect() * block_size) + outerPosition, SEEK_SET);
                if (fread(&outerBlockIndexChar, 1, 1, sim_disk_fd) != 1)
                {
                    cerr << "Error reading outer indirect block index." << endl;
                    delete[] blocks_indexes;
                    return -1;
                }
                int outerBlockIndex = charToDec(outerBlockIndexChar);
                if (outerBlockIndex == -1)
                {
                    continue;
                }

                char innerBlockIndexChar;
                for (int innerPosition = 0; innerPosition < block_size; innerPosition++) // Assuming block size is the maximum number of entries in inner block
                {
                    fseek(sim_disk_fd, (outerBlockIndex * block_size) + innerPosition, SEEK_SET);
                    if (fread(&innerBlockIndexChar, 1, 1, sim_disk_fd) != 1)
                    {
                        cerr << "Error reading inner indirect block index." << endl;
                        delete[] blocks_indexes;
                        return -1;
                    }
                    int innerBlockIndex = charToDec(innerBlockIndexChar);
                    if (innerBlockIndex == -1)
                    {
                        continue;
                    }
                    if(i<fileToUse->getBlockInUse()){
                        blocks_indexes[i] = innerBlockIndex;
                    }
                    i++;
                }
            }
        }

        // Read from the determined block indexes.
        memset(buf, 0, len);
        int bytesRead = 0;
        for (int j = 0; j < i && bytesRead < max_len && bytesRead<len; j++) {
            char *to_read = new char[block_size];
            fseek(sim_disk_fd, blocks_indexes[j] * block_size, SEEK_SET);

            int l = (j == i - 1) ? (max_len - bytesRead) : block_size;  // Determine the length to read.

            fread(to_read, 1, l, sim_disk_fd);
            memcpy(&buf[bytesRead], to_read, l);

            bytesRead += l;
            delete[] to_read;
        }
        buf[len] = '\0';
        delete[] blocks_indexes;
        return 1;  // return 1 for success
    }


    // ------------------------------------------------------------------------
    int GetFileSize(int fd)
    {
        return this->OpenFileDescriptors[fd].GetFileSize();
    }


    // ------------------------------------------------------------------------
    int CopyFile(string srcFileName, string destFileName)
    {
        /*if (srcFileName == destFileName)
        {
            cerr << "Source file and destination file names are similar\n"<< endl;
            return -1;
        }*/
        // Check if the source file exists in MainDir
        auto itSrc = MainDir.find(srcFileName);
        if (itSrc == MainDir.end())
        {
            cerr << "Source file does not exist!"<< endl;
            return -1;
        }

        /*****************check if the files are closed !!!!!!************************/
        int i = 0;
        for (i = 0; i < (int)this->OpenFileDescriptors.size(); i++)
        {
            if (this->OpenFileDescriptors[i].getFileName() == srcFileName && this->OpenFileDescriptors[i].isInUse() &&
                    !(this->OpenFileDescriptors[i].getDeletedStatus()))
            {
                cerr << "you cannot copy an opened file!!"<<endl;
                return -1;
            }
        }

        // Check if the destination file already exists
        auto itDest = MainDir.find(destFileName);
        if (itDest != MainDir.end())
        {
//            destFileName = destFileName + "(1)";
            destFileName = GenerateNewName(MainDir, destFileName);

        }
        // Create the destination file using CreateFile
        int destFd = CreateFile(destFileName);
        if (destFd == -1)
        {
            cerr << "Failed to create destination file!"<<endl;
            return -1;
        }

        // Read the source file into a buffer
        int srcSize = itSrc->second->getFileSize();
        /*****************check if the disk can talk all the new file !!!!!!************************/

        char *copyBuffer = new char[srcSize];
        int srcFd = OpenFile(srcFileName);
        if (srcFd == -1 || ReadFromFile(srcFd, copyBuffer, srcSize) == -1)
        {
            cerr << "Failed to read source file!" << endl;
            delete[] copyBuffer;
            return -1;
        }

        // Write the buffer contents into the destination file
        if (WriteToFile(destFd, copyBuffer, srcSize) == -1)
        {
            cerr << "Failed to write to destination file!" << endl;
            delete[] copyBuffer;
            return -1;
        }

        // Close the source file (if needed)
        CloseFile(srcFd);
        CloseFile(destFd);
        // Cleanup
        delete[] copyBuffer;

        // Copy successful
        return 1;
    }

    // ------------------------------------------------------------------------
    int RenameFile(string oldFileName, string newFileName)
    {
        if (!(this->is_formated))
        { // check if the disk has been formated, if not return -1
            cerr << "the disk has not formated!!"<<endl;
            return -1;
        }
        /**check if the file is not exist by using find function and return -1 if its not exist.**/
        map<string, fsInode *>::iterator itr;
        itr = this->MainDir.find(oldFileName);
        if (itr == this->MainDir.end())
        {
            cerr << "the file is not exist!!"<<endl;
            return -1;
        }
        int i = 0;
        for (i = 0; i < (int)this->OpenFileDescriptors.size(); i++)
        {
            if (this->OpenFileDescriptors[i].getFileName() == oldFileName && (this->OpenFileDescriptors[i].isInUse() ||
                                                                              this->OpenFileDescriptors[i].getDeletedStatus()))
            {
                cerr << "you cannot rename an opened file!!"<<endl;
                return -1;
            }
            if (this->OpenFileDescriptors[i].getFileName() == oldFileName && !this->OpenFileDescriptors[i].isInUse() &&
                !this->OpenFileDescriptors[i].getDeletedStatus())
            {
                this->OpenFileDescriptors[i].setFileName(newFileName);
                break;
            }
        }
        fsInode *inode_ptr = itr->second;

        // Erase the old key-value pair from the map
        this->MainDir.erase(itr);

        // Insert the new key-value pair into the map
        this->MainDir[newFileName] = inode_ptr;

        return i;
    }

    ~fsDisk()
    {
        if (this->MainDir.empty())
        {
            // Close the sim_disk_fd if it's open
            if (sim_disk_fd)
            {
                fclose(sim_disk_fd);
            }
            return;
        }

        // Deallocate BitVector
        delete[] BitVector;
        // Delete all the fsInode pointers in MainDir
        for (auto &entry : MainDir)
        {
            delete entry.second;
        }
        MainDir.clear();

        // No need to delete anything from OpenFileDescriptors, because it contains
        // objects, not pointers. Also, the fsInode pointers in OpenFileDescriptors
        // are also stored in MainDir, and we've already deleted them.

        // Clear OpenFileDescriptors
        OpenFileDescriptors.clear();

        /********************************************************************************/
        // Fill a buffer with 512 null bytes
        char nullBuffer[DISK_SIZE];
        memset(nullBuffer, 0, sizeof(nullBuffer));

        // Go to the beginning of the file
        fseek(sim_disk_fd, 0, SEEK_SET);

        // Write the null bytes to the file
        fwrite(nullBuffer, 1, sizeof(nullBuffer), sim_disk_fd);

        // Flush any buffered output for the given output or update stream
        fflush(sim_disk_fd);

        // Close the sim_disk_fd if it's open
        if (sim_disk_fd)
        {
            fclose(sim_disk_fd);
        }
    }

private:
    /** the function return the first empty block from the BitVector , if an error occurred or there ar no empty blocks return -1 **/
    int firstEmptyBlock()
    {
        for (int i = 0; i < this->BitVectorSize; i++)
        {
            if (this->BitVector[i] == 0)
            {
                this->BitVector[i] = 1;
                return i;
            }
        }
        return -1;
    }

    /** the function count how many empty blocks we have in the disk **/
    int emptyBlocksCounter()
    {
        int emptyBlocksCounter = 0;
        for (int i = 0; i < this->BitVectorSize; i++)
        {
            if (this->BitVector[i] == 0)
            {
                emptyBlocksCounter++;
            }
        }
        return emptyBlocksCounter;
    }

    /** the function return the first empty block from the end of the BitVector , if an error occurred or there ar no empty blocks return -1 **/
    int findEmptyBlockFromTheEnd()
    {
        int i;
        for (i = BitVectorSize - 1; i >= 0; i--)
        {
            if (this->BitVector[i] == 0)
            {
                this->BitVector[i] = 1;
                return i;
            }
        }
        return -1;
    }

    /** the function count how much data we have in the disk in all files**/
    /*int dataCounter()
    {
        int dataCounter = 0;
        for (const auto &pair : MainDir)
        {
            fsInode *fsI = pair.second;
            dataCounter += fsI->getFileSize();
        }
        return dataCounter;
    }*/

    int getCurrentPointerBlockIndex(fsInode *inode)
    {   int remainCharsForDouble=inode->getFileSize()-((3*block_size)+(block_size*block_size));

        if (inode->getDoubleInDirect() != -1)
        {
            char *firstIndx = new char(1);
            int outerPosition = remainCharsForDouble / (block_size * block_size);
            fseek(sim_disk_fd, (inode->getDoubleInDirect() * block_size) + outerPosition, SEEK_SET);
            if (fread(firstIndx, 1, 1, sim_disk_fd) != 1)
            {
                cerr << "error reading outer indirect block func: getCurrentPointerBlockIndex()" << endl;
                delete firstIndx;
                return -1;
            }
            int innerBlock = charToDec(*firstIndx);
            delete firstIndx;

            char *secondIndx = new char(1);
            int innerPosition = (remainCharsForDouble % (block_size * block_size)) / block_size;
            fseek(sim_disk_fd, (innerBlock * block_size) + innerPosition, SEEK_SET);
            if (fread(secondIndx, 1, 1, sim_disk_fd) != 1)
            {
                cerr << "error reading inner indirect block func: getCurrentPointerBlockIndex()" << endl;
                delete secondIndx;
                return -1;
            }
            int toReturn = charToDec(*secondIndx);
            delete secondIndx;
            return toReturn;
        }
        if (inode->getSingleInDirect() != -1)
        {
            char *indx = new char(1);
            int position = inode->getFileSize() / block_size - 3;
            fseek(sim_disk_fd, (inode->getSingleInDirect() * block_size) + position, SEEK_SET);
            if (fread(indx, 1, 1, sim_disk_fd) != 1)
            {
                cerr << "some error reading from file func: getCurrentPointerBlockIndex()" << endl;
                return -1;
            }
            int to_return = charToDec(*indx);
            delete indx;
            return to_return;
        }
        if (inode->getDirectBlock3() != -1)
        {
            return inode->getDirectBlock3();
        }
        if (inode->getDirectBlock2() != -1)
        {
            return inode->getDirectBlock2();
        }
        return inode->getDirectBlock1();
    }

    void setNextAvailableBlockIndex(fsInode *inode, int index)
    {   int remainCharsForDouble=inode->getFileSize()-((3*block_size)+(block_size*block_size));
        int innerPosition = (remainCharsForDouble % (block_size * block_size)) / block_size;

        if (inode->getDirectBlock1() == -1)
        {
            inode->setDirectBlock1(index);
            inode->incrementBlockInUse();
        }
        else if (inode->getDirectBlock2() == -1)
        {
            inode->setDirectBlock2(index);
            inode->incrementBlockInUse();
        }
        else if (inode->getDirectBlock3() == -1)
        {
            inode->setDirectBlock3(index);
            inode->incrementBlockInUse();
        }
        else if (inode->getSingleInDirect() == -1)
        {
            inode->setSingleInDirect(index);
            int next_block = this->findEmptyBlockFromTheEnd();
            char char_to_write = decToBinary(next_block);
            fseek(sim_disk_fd, index * block_size, SEEK_SET);
            fwrite(&char_to_write, 1, 1, sim_disk_fd);
            inode->incrementBlockInUse();
        }
        else if (inode->getSingleInDirect() != -1 && inode->getFileSize() < (block_size * 3) + block_size * block_size)
        {
            int next_block = index;
            int single_in_direct = inode->getSingleInDirect();
            char char_to_write = decToBinary(next_block);
            int position = (single_in_direct * block_size) + ((inode->getFileSize() / block_size) - 3);
            fseek(sim_disk_fd, position, SEEK_SET);
            fwrite(&char_to_write, 1, 1, sim_disk_fd);
            inode->incrementBlockInUse();
        }
        else if (inode->getDoubleInDirect() == -1)
        {
            inode->setDoubleInDirect(index);
            int next_outer_block = this->findEmptyBlockFromTheEnd();
            char char_to_write_outer = decToBinary(next_outer_block);
            fseek(sim_disk_fd, index * block_size, SEEK_SET);
            fwrite(&char_to_write_outer, 1, 1, sim_disk_fd);

            int next_inner_block = this->findEmptyBlockFromTheEnd();
            char char_to_write_inner = decToBinary(next_inner_block);
            fseek(sim_disk_fd, next_outer_block * block_size, SEEK_SET);
            fwrite(&char_to_write_inner, 1, 1, sim_disk_fd);

            // Increment again for the inner block.
            inode->incrementBlockInUse();
        }
        else if (inode->getDoubleInDirect() != -1)
        {
            int emptyBlockFromTheEnd = index;
            int double_in_direct = inode->getDoubleInDirect();
            char char_to_write_outer = decToBinary(emptyBlockFromTheEnd);
            int outerPosition;
            if(innerPosition==0 && remainCharsForDouble%(block_size*block_size)==0 && remainCharsForDouble<(block_size*block_size*block_size)){
                outerPosition = (double_in_direct * block_size) + ((remainCharsForDouble / (block_size * block_size)));
                fseek(sim_disk_fd, outerPosition, SEEK_SET);
                fwrite(&char_to_write_outer, 1, 1, sim_disk_fd);
                int positionOfFirstZero = findFirstZeroIndex(sim_disk_fd, inode->getDoubleInDirect()*block_size);
                int next_inner_block;
                next_inner_block=findEmptyBlockFromTheEnd();
                char char_to_write_inner = decToBinary(next_inner_block);

                fseek(sim_disk_fd, (positionOfFirstZero*block_size)+innerPosition, SEEK_SET);
                fwrite(&char_to_write_inner, 1, 1, sim_disk_fd);

                // Increment again for the inner block.
                inode->incrementBlockInUse();
            }
            else{
                int positionOfFirstZero = findFirstZeroIndex(sim_disk_fd, inode->getDoubleInDirect()*block_size);
                int next_inner_block;
                next_inner_block = index;
                char char_to_write_inner = decToBinary(next_inner_block);

                fseek(sim_disk_fd, (positionOfFirstZero*block_size)+innerPosition, SEEK_SET);
                fwrite(&char_to_write_inner, 1, 1, sim_disk_fd);

                // Increment again for the inner block.
                inode->incrementBlockInUse();
            }

        }
    }
    int findFirstZeroIndex(FILE *sim_disk_fd1, int initialPosition) {
        char currentChar;
        char previousChar;
        int currentPosition = initialPosition;

        while (true) {
            // Loop until you find '\0'
            fseek(sim_disk_fd, currentPosition, SEEK_SET);
            for (int i = 0; i < block_size; i++) {
                if (fread(&currentChar, 1, 1, sim_disk_fd) != 1) {
                    cerr << "Error reading character from file." << endl;
                    return -1; // Indicate an error
                }
                if (currentChar == '\0') {
                    break;
                }
                previousChar = currentChar;
            }

            // Save the last character before '\0'
            int nextPosition = charToDec(previousChar);

            // Loop to move to the new position
            currentPosition = nextPosition;

            // Read until you find the first occurrence of '\0'
            fseek(sim_disk_fd, currentPosition, SEEK_SET);
            for (int i = 0; i < block_size; i++) {
                if (fread(&currentChar, 1, 1, sim_disk_fd) != 1) {
                    cerr << "Error reading character from file." << endl;
                    return -1; // Indicate an error
                }
                if (currentChar == '\0') {
                    // Return the position/index of the first occurrence of '\0'
                    return currentPosition + i;
                }
            }

            // Optional: Add a condition here to break out of the loop if necessary.
            // For this demonstration, I'll use the following condition to avoid infinite loops:
            if (nextPosition == currentPosition) {
                break;
            }
        }

        return -1; // If the function hasn't returned before this point, return an error
    }

    static int CountPrefixMatches(const std::map<std::string, fsInode*>& directory, const std::string& prefix) {
        int count = 0;
        for (const auto & it : directory) {
            if (it.first.compare(0, prefix.size(), prefix) == 0) {
                count++;
            }
        }
        return count;
    }

    static std::string GenerateNewName(const std::map<std::string, fsInode*>& directory, const std::string& name) {
        std::string newName = name;
        int counter = CountPrefixMatches(directory, name);
        if (counter > 0) {
            std::ostringstream oss;
            oss << name << "(" << counter << ")";
            newName = oss.str();
        }
        return newName;
    }


};

int main() {
    int blockSize;
    string fileName;
    string fileName2;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(true) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
                delete fs;
                exit(0);
                break;

            case 1:  // list-file
                fs->listAll();
                break;

            case 2:    // format
                cin >> blockSize;
                fs->fsFormat(blockSize);
                break;

            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4:  // open-file
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5:  // close-file
                cin >> _fd;
                fileName = fs->CloseFile(_fd);
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
                break;

            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read ;
                fs->ReadFromFile( _fd , str_to_read , size_to_read );
                cout << "ReadFromFile: " << str_to_read << endl;
                break;

            case 8:   // delete file
                cin >> fileName;
                _fd = fs->DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 9:   // copy file
                cin >> fileName;
                cin >> fileName2;
                fs->CopyFile(fileName, fileName2);
                break;

            case 10:  // rename file
                cin >> fileName;
                cin >> fileName2;
                fs->RenameFile(fileName, fileName2);
                break;

            default:
                break;
        }
    }

}
