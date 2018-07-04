#ifndef DEXDBEXCEPTION_H
#define DEXDBEXCEPTION_H

#include <exception>

using namespace std;

class DexDBException : public exception
{
public:
    explicit DexDBException(const int status) : status(status) {}

    virtual const char* what() const throw() {
        return message();
    }

private:
    const int status;
    const char *message() const {
        const char *msg;
        switch (status) {
        case SQLITE_ERROR:
            msg = "\nGeneric error\n";
            break;
        case SQLITE_INTERNAL:
            msg = "\nInternal logic error in SQLite\n";
            break;
        case SQLITE_PERM:
            msg = "\nAccess permission denied\n";
            break;
        case SQLITE_ABORT:
            msg = "\nCallback routine requested an abort\n";
            break;
        case SQLITE_BUSY:
            msg = "\nThe database file is locked\n";
            break;
        case SQLITE_LOCKED:
            msg = "\nA table in the database is locked\n";
            break;
        case SQLITE_NOMEM:
            msg = "\nA malloc() failed\n";
            break;
        case SQLITE_READONLY:
            msg = "\nAttempt to write a readonly database\n";
            break;
        case SQLITE_INTERRUPT:
            msg = "\nOperation terminated by sqlite3_interrupt()\n";
            break;
        case SQLITE_IOERR:
            msg = "\nSome kind of disk I/O error occurred\n";
            break;
        case SQLITE_CORRUPT:
            msg = "\nThe database disk image is malformed\n";
            break;
        case SQLITE_NOTFOUND:
            msg = "\nUnknown opcode in sqlite3_file_control()\n";
            break;
        case SQLITE_FULL:
            msg = "\nInsertion failed because database is full\n";
            break;
        case SQLITE_CANTOPEN:
            msg = "\nUnable to open the database file\n";
            break;
        case SQLITE_PROTOCOL:
            msg = "\nDatabase lock protocol error\n";
            break;
        case SQLITE_EMPTY:
            msg = "\nInternal use only\n";
            break;
        case SQLITE_SCHEMA:
            msg = "\nThe database schema changed\n";
            break;
        case SQLITE_TOOBIG:
            msg = "\nString or BLOB exceeds size limit\n";
            break;
        case SQLITE_CONSTRAINT:
            msg = "\nAbort due to constraint violation\n";
            break;
        case SQLITE_MISMATCH:
            msg = "\nData type mismatch\n";
            break;
        case SQLITE_MISUSE:
            msg = "\nLibrary used incorrectly\n";
            break;
        case SQLITE_NOLFS:
            msg = "\nUses OS features not supported on host\n";
            break;
        case SQLITE_AUTH:
            msg = "\nAuthorization denied\n";
            break;
        case SQLITE_FORMAT:
            msg = "\nNot used\n";
            break;
        case SQLITE_RANGE:
            msg = "\n2nd parameter to sqlite3_bind out of range\n";
            break;
        case SQLITE_NOTADB:
            msg = "\nFile opened that is not a database file\n";
            break;
        case SQLITE_NOTICE:
            msg = "\nNotifications from sqlite3_log()\n";
            break;
        case SQLITE_WARNING:
            msg = "\nWarnings from sqlite3_log()\n";
            break;
        case SQLITE_ROW:
            msg = "\nsqlite3_step() has another row ready\n";
            break;
        case SQLITE_DONE:
            msg = "\nsqlite3_step() has finished executing\n";
            break;
        default:
            msg = "\nUnknown error\n";
            break;
        }

        return msg;
    }
};

#endif //DEXDBEXCEPTION_H
