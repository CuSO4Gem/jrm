#include "MdJournalIO.h"

#include <regex>
#include <unistd.h>

/*debug */
#include "debug_print.h"

void md2txtEnter(string &iStr)
{
    if (iStr.length() == 0)
        return;
    if (iStr.length()>2 && iStr[iStr.length()-2]=='\r')
        iStr.erase(iStr.length()-2, 1);
    if (iStr.length()>2 && iStr.substr(iStr.length()-2,2)=="  ")
        iStr = iStr.substr(0, iStr.length()-2) + string("\n"); 
}

void txt2mdEnter(string &iStr)
{
    if (iStr.length()>2 && iStr[iStr.length()-2]=='\r')
        iStr.erase(iStr.length()-2, 1);
    if (iStr.length() == 0)
        return;
    size_t i = 0;
    while (i<iStr.length())
    {
        
        if (iStr[i]=='\r')
            iStr.erase(i, 1);
        if (iStr[i]=='\n')
        {
            iStr.insert(i, "  ");
            i += 3;
        }
        else
            i++;    
    }
}

MdJournalIO::MdJournalIO()
{
    mState = UNINITED;
}

MdJournalIO::~MdJournalIO()
{
    if (mJournal.is_open())
        mJournal.close();
}

uint32_t MdJournalIO::apiSupport()
{
    return 1;
}

vector<string> MdJournalIO::formateSupport()
{
    list<string> formates {string("txt")};
    return formates;
}

bool MdJournalIO::isSupportAes256()
{
    return false;
}

void MdJournalIO::setKey(uint8_t key[32])
{
    return;
}

void MdJournalIO::clearKey()
{
    return;
}

bool MdJournalIO::setReadMod()
{
    if(mState==UNINITED)
        return false;

    if (mJournal.is_open())
        mJournal.close();

    mJournal.open(mJournalPath, ios::in);
    if (!mJournal)
        return false;

    mJournal.seekg(0, ios::end);
    mFileSize = mJournal.tellg();
    mJournal.seekg(0, ios::beg);
    mState = READ;
    return true;
}

bool MdJournalIO::setWriteMode()
{
    if(mState==UNINITED)
        return false;

    if (mJournal.is_open())
        mJournal.close();

    mJournal.open(mJournalPath, ios::out);
    if (!mJournal)
        return false;

    mState = WRITE;
    return true;
}

bool MdJournalIO::open(string path)
{
    mJournalPath = path;
    mJournal.open(mJournalPath, ios::in);
    if (!mJournal)
    {
        /*maybe file no exist, try too create one*/
        string cmd = string("touch ")+path;
        JLOGT("touch journal file: %s\n",cmd.c_str());
        int ret = system(cmd.c_str());
        if (ret!=0)
            return false;
        else
        {
            mState = INITED;
            return true;
        }
    }
    
    /*an empty file, just rewrite it*/
    mJournal.seekg(0, ios::end);
    if (0==mJournal.tellg())
    {
        mJournal.close();
        mState = INITED;
        return true;
    }
    mJournal.seekg(0, ios::beg);
    
    /*check there have at list one journal in file*/
    string lineBuffer;
    smatch regexResult;
    bool finded;
    finded = false;
    //todo: more strong, some journal may leak some element
    while (getline(mJournal, lineBuffer))
    {
        size_t prLen=0;
        if (lineBuffer.length()>2 && lineBuffer[0]=='#' && lineBuffer[1]==' ')
        {
            finded = true;
            break;
        }
    }
    if (!finded)
    {
        mJournal.close();
        return false;
    }
    //config begin
    finded = false;
    while (getline(mJournal, lineBuffer))
    {
        size_t prLen=0;
        if (lineBuffer.length()>=3
        && lineBuffer[0]=='`' && lineBuffer[1]=='`' && lineBuffer[2]=='`')
        {
            finded = true;
            break;
        }
    }
    if (!finded)
    {
        mJournal.close();
        return false;
    }
    //config end
    finded = false;
    while (getline(mJournal, lineBuffer))
    {
        size_t prLen=0;
        if (lineBuffer.length()>=3
        && lineBuffer[0]=='`' && lineBuffer[1]=='`' && lineBuffer[2]=='`')
        {
            finded = true;
            break;
        }
    }
    if (!finded)
    {
        mJournal.close();
        return false;
    }
    //note need to find end ======

    mJournal.close();
    mState = INITED;
    return true;
}

void MdJournalIO::close()
{
    mJournal.close();
    mState = UNINITED;
}

shared_ptr<Journal> MdJournalIO::readJournal()
{
    if (!mJournal.is_open() || mState!=READ)
        return nullptr;
        
    shared_ptr<Journal> journl = make_shared<Journal>();
    string lineBuffer;
    string readBuffer;
    smatch regexResult;
    bool finded;
    size_t i, getSpace;
    finded = false;
    //todo: more strong
    while (getline(mJournal, lineBuffer))
    {
        if (lineBuffer.length()>2 && lineBuffer[0]=='#' && lineBuffer[1]==' ')
        {
            finded = true;
            lineBuffer = lineBuffer.substr(2, lineBuffer.length()-2);
            break;
        }
    }
    if (!finded)
    {
        JLOGE("ERROR: return null journal while search title");
        mJournal.close();
        mState = INITED;
        return nullptr;
    }

    /*read title*/
    finded = false;
    readBuffer.clear();
    readBuffer.append(lineBuffer);
    while (getline(mJournal, lineBuffer))
    {
        if (lineBuffer.length()>=3
        && lineBuffer[0]=='`' && lineBuffer[1]=='`' && lineBuffer[2]=='`')
        {
            finded = true;
            break;
        }
        else
        {
            md2txtEnter(lineBuffer);
            readBuffer.append(lineBuffer);
        }
    }
    if (!finded)
    {
        JLOGE("ERROR: return null journal while reading title");
        mJournal.close();
        mState = INITED;
        return nullptr;
    }
    i = 0;
    getSpace = 0;
    while (i<readBuffer.length())
    {
        if (readBuffer[i] == '\n' && i<readBuffer.length()-1)
            readBuffer[i] = ' ';

        if (readBuffer[i] == ' ')
            getSpace++;
        else
            getSpace = 0;

        if (getSpace>1 || readBuffer[i]=='\t')
            readBuffer.erase(i, 1);
        else
            i++;    
    }
    if (readBuffer[readBuffer.length()-1] != '\n')
        readBuffer.append("\n");
    journl->setTitle(readBuffer);

    /*read config*/
    finded = false;
    readBuffer.clear();
    while (getline(mJournal, lineBuffer))
    {
        if (lineBuffer.length()>=3
        && lineBuffer[0]=='`' && lineBuffer[1]=='`' && lineBuffer[2]=='`')
        {
            finded = true;
            break;
        }
        
        /* ignore the line wich without any content*/
        bool haveContent = false;
        for (size_t i = 0; i < lineBuffer.length(); i++)
        {
            if (lineBuffer[i] != ' ' &&
                lineBuffer[i] != '\t' &&
                lineBuffer[i] != '\r' &&
                lineBuffer[i] != '\n')
            {
                haveContent = true;
                break;
            }
        }
        
        if (haveContent)
            readBuffer.append(lineBuffer + string("\n"));
    }
    if (!finded)
    {
        JLOGE("ERROR: return null journal while reading config");
        mJournal.close();
        mState = INITED;
        return nullptr;
    }
    if (readBuffer[readBuffer.length()-1] != '\n')
        readBuffer.append("\n");
    journl->setConfig(readBuffer);

    /*read content*/
    finded = false;
    readBuffer.clear();
    while (getline(mJournal, lineBuffer))
    {
        if (lineBuffer.length()>=3 && 
        lineBuffer[0] == '*' && lineBuffer[1] == '*' && lineBuffer[2] == '*')
        {
            break;
        }
        else
        {
            md2txtEnter(lineBuffer);
            readBuffer.append(lineBuffer);
        }
    }
    if (mJournal.tellg() == mFileSize)
    {
        mJournal.close();
        mState = INITED;
    }
    if (readBuffer[readBuffer.length()-1] != '\n')
        readBuffer.append("\n");
    journl->setContent(readBuffer);

    return journl;
}

bool MdJournalIO::writeJournal(shared_ptr<Journal> journal)
{
    if (!mJournal.is_open() || mState!=WRITE)
        return false;

    string title = string("# ") + journal->getTitle();
    size_t i = 0;
    size_t getSpace = 0;
    /**
     * markdown title is only one line. convert '\n' to ' '
     * and remove mutly space
     * */
    while (i<title.length())
    {
        if (title[i] == '\n' && i<title.length()-1)
            title[i] = ' ';

        if (title[i] == ' ')
            getSpace++;
        else
            getSpace = 0;

        if (getSpace>1 || title[i]=='\r')
            title.erase(i, 1);
        else
            i++;    
    }
    mJournal.write(title.c_str(), title.length());
    if (title[title.length()-1]!='\n')
        mJournal.write("\n", 1);

    string config = journal->getConfig();
    string lineBuffer;
    lineBuffer = string("```\n");
    mJournal.write(lineBuffer.c_str(), lineBuffer.length());
    mJournal.write(config.c_str(), config.length());
    if (config[config.length()-1]!='\n')
        mJournal.write("\n", 1);
    mJournal.write(lineBuffer.c_str(), lineBuffer.length());

    string content = journal->getContent();
    txt2mdEnter(content);
    mJournal.write(content.c_str(), content.length());
    if (content[content.length()-1]!='\n')
        mJournal.write("\n", 1);
    lineBuffer = string("***\n");
    mJournal.write(lineBuffer.c_str(), lineBuffer.length());
    return true;
}
