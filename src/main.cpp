#include <memory>
#include <stdio.h>
#include <stdint.h>

#include <string>

using namespace std;

#include "TxtJournalIO.h"
#include "KiloEditor.h"
#include "JournalIOBase.h"
#include "JrmeInstall.h"

int main(int argc, char* argv[]) {
    bool ret;
    ret = installIfNeed();
    printf("installIfNeed ret=%d\n", ret);

    shared_ptr<JournalIOBase> jIO(new TxtJournalIO());
    return 0;
}