#include "SfJournalBook.h"

bool SfJournalBook::open(string path)
{
    //todo : add more formate of JournalIO
    mJournalIO = shared_ptr<JournalIOBase>(new TxtJournalIO());
    if (!mJournalIO->open(path))
        return false;
    
    shared_ptr<Journal> journal;
    while ((journal=mJournalIO->readJournal()) != nullptr)
    {
        mJournalList.push_back(journal);
    }
    
    return true;
}

void SfJournalBook::sort(int32_t type)
{
    ;
}

bool SfJournalBook::save()
{
    return true;
}

size_t SfJournalBook::size()
{
    AutoLock aLock = AutoLock(mJournalListLock);
    return mJournalList.size();
}

shared_ptr<Journal> SfJournalBook::at(size_t pos)
{
    AutoLock aLock = AutoLock(mJournalListLock);
    if (pos >= mJournalList.size())
        return nullptr;
    
    list<shared_ptr<Journal>>::iterator it = mJournalList.begin();
    for (size_t i = 0; i < pos; i++)
    {
        it++;
    }

    return *(it);
}

shared_ptr<Journal> SfJournalBook::operator [](size_t pos)
{
    AutoLock aLock = AutoLock(mJournalListLock);
    if (pos >= mJournalList.size())
        return nullptr;

    list<shared_ptr<Journal>>::iterator it = mJournalList.begin();
    for (size_t i = 0; i < pos; i++)
    {
        it++;
    }

    return *(it);
}

bool SfJournalBook::insert(size_t pos, shared_ptr<Journal> journal)
{
    AutoLock aLock = AutoLock(mJournalListLock);
    if (pos<0 || pos>mJournalList.size())
        return false;

    //todo : test
    if (pos==mJournalList.size())
    {
        mJournalList.push_back(journal);
        return true;
    }

    list<shared_ptr<Journal>>::iterator it = mJournalList.begin();
    for (size_t i = 0; i < pos; i++)
    {
        it++;
    }
    
    mJournalList.insert(it, journal);
    return true;
}

void SfJournalBook::push_front(shared_ptr<Journal> journal)
{
    AutoLock aLock = AutoLock(mJournalListLock);
    mJournalList.push_front(journal);
}

void SfJournalBook::push_back(shared_ptr<Journal> journal)
{
    AutoLock aLock = AutoLock(mJournalListLock);
    mJournalList.push_back(journal);
}

bool SfJournalBook::swap(size_t pos1, size_t pos2)
{
    AutoLock aLock = AutoLock(mJournalListLock);
    if (pos1<0 || pos1>=mJournalList.size()||
        pos2<0 || pos2>=mJournalList.size())
        return false;
    
    list<shared_ptr<Journal>>::iterator it1 = mJournalList.begin();
    for (size_t i = 0; i < pos1; i++)
    {
        it1++;
    }

    list<shared_ptr<Journal>>::iterator it2 = mJournalList.begin();
    for (size_t i = 0; i < pos2; i++)
    {
        it2++;
    }

    iter_swap(it1, it2);
    return true;
}
