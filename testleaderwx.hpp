#pragma once
#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/symbol.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>

using namespace std;

#define CONTRACTN eosio::name("testleaderwx");

CONTRACT testleaderwx : public eosio::contract {
  public:
    using contract::contract;

    ACTION createlboard(uint64_t id, string date, string game);
    ACTION resetlboard(uint64_t boardid);
    ACTION removelboard(uint64_t boardid);
    ACTION updatelboard(uint64_t boardid, eosio::name username, uint64_t points);

  private:

    struct player_s
    {
      eosio::name account;      //user wax account
      uint64_t points;          //user points
    };

    TABLE lboard {
      uint64_t          id;         //leaderboard id on table
      string            date;       //date of leaderboard
      string            game;       //game of leaderboard
      vector<player_s>  players;    //array of players

      uint64_t primary_key() const { return id; }
    };
    typedef eosio::multi_index<eosio::name("lboards"), lboard> lboards;

  public:
  
  testleaderwx(eosio::name receiver, eosio::name code, eosio::datastream<const char *> ds) : contract(receiver, code, ds) {}
  uint64_t finder(vector<player_s> players, eosio::name username);
   
};