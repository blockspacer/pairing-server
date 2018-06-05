#ifndef _DATABASE_H
#define _DATABASE_H

#include <exception>
#include <postgresql/libpq-fe.h>
#include <string>
#include <vector>

#include "types.pb.h"

class Database {
    public:
        Database(const char *dbname, const char *user, const char *password,
                const char *host = "127.0.0.1");
        ~Database();

        void begin();
        void commit();
        void rollback();

        template<typename Func>
        void transaction(Func cb) {
            begin();
            try {
                cb();
            }
            catch(...) {
                rollback();
                throw;
            }
            commit();
        }

        void getTournament(pairing_server::Tournament *t);
        void insertTournament(pairing_server::Tournament *t);
        std::vector<pairing_server::TournamentPlayer> tournamentPlayers(pairing_server::Identification *id);

        void insertPlayer(pairing_server::TournamentPlayer *p);
    private:
        PGconn *db;
        void prepare(const char *name, const char *sql, int count);
        PGresult *execute(const char *stmt, int count, const char **values,
                const int *lengths, const int *formats, int resultFormat);
        void sqlDo(const char *sql);
};

class DatabaseError : public std::exception {
    public:
        DatabaseError(const char *dbmsg) : msg(dbmsg) {}
        const char *what() const noexcept { return msg.c_str(); }

    private:
        std::string msg;
};

#endif
