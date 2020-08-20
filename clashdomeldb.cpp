#include "clashdomeldb.hpp"

// An action for creating new leader board
// owner - owner's wax account
// boardname - name for leaderboard
// gameid - id of owner's game
void clashdomeldb::createlboard(uint64_t id, string date, string game)
{
    require_auth(_self);

    lboards _lb(CONTRACTN, CONTRACTN.value);

    auto lb_itr = _lb.find(id);
    eosio::check(lb_itr == _lb.end(), "Leader board with " + to_string(id) + " id already exists!");

    _lb.emplace(CONTRACTN, [&](auto& new_board) {
        new_board.id = id;
        new_board.date = date;
        new_board.game = game;
    });
}

// An action for removing leaderboard from table
// owner - owner wax account
// boardid - leaderboard id on table lboards
void clashdomeldb::removelboard(uint64_t boardid)
{
    require_auth(_self);

    lboards _lb(CONTRACTN, CONTRACTN.value);

    auto lb_itr = _lb.find(boardid);
    eosio::check(lb_itr != _lb.end(), "Leader board with " + to_string(boardid) + " id doesn't exist!");

    _lb.erase(lb_itr);
}

// An action for reseting leaderboard
// owner - owner wax account
// boardid - leaderboard id on table lboards
// resetpool - 0: pot on leaderboard stay current, 1: pot on leaderboard change to 0
void clashdomeldb::resetlboard(uint64_t boardid)
{
    require_auth(_self);

    lboards _lb(CONTRACTN, CONTRACTN.value);

    auto lb_itr = _lb.find(boardid);
    eosio::check(lb_itr != _lb.end(), "Leader board with " + to_string(boardid) + " id doesn't exist!");

    _lb.modify(lb_itr, get_self(), [&](auto &mod_board) {
        mod_board.players.clear();
    });
}

// An action for update/create a user on leaderboard
// owner - owner wax account
// boardid - leaderboard id on table lboards
// username - user's wax account
// point - number of points to add
void clashdomeldb::updatelboard(uint64_t boardid, eosio::name username, uint64_t points, uint64_t upload_time)
{
    require_auth(_self);

    lboards _lb(CONTRACTN, CONTRACTN.value);

    auto lb_itr = _lb.find(boardid);
    eosio::check(lb_itr != _lb.end(), "Leader board with " + to_string(boardid) + " id doesn't exist!");

    uint64_t pos = finder(lb_itr->players, username);
    if (pos == -1)
    {
        player_s helper;
        pos = lb_itr->players.size();
        _lb.modify(lb_itr, get_self(), [&](auto &mod_board) {
            mod_board.players.push_back({
                username,
                points,
                upload_time
            });
            while (pos != 0)
            {
                if (mod_board.players.at(pos).points > mod_board.players.at(pos - 1).points)
                {
                    helper = mod_board.players.at(pos);
                    mod_board.players.at(pos) = mod_board.players.at(pos - 1);
                    mod_board.players.at(pos - 1) = helper;

                    pos--;
                }
                else
                {
                    break;
                }
            }
        });
    }
    else
    {
        player_s helper;
        _lb.modify(lb_itr, get_self(), [&](auto &mod_board) {
            if (points > mod_board.players.at(pos).points) {
                mod_board.players.at(pos).points = points;
            }
            while (pos != 0)
            {
                if (mod_board.players.at(pos).points > mod_board.players.at(pos - 1).points)
                {
                    helper = mod_board.players.at(pos);
                    mod_board.players.at(pos) = mod_board.players.at(pos - 1);
                    mod_board.players.at(pos - 1) = helper;

                    pos--;
                }
                else if (mod_board.players.at(pos).upload_time < mod_board.players.at(pos - 1).upload_time)
                {
                    helper = mod_board.players.at(pos);
                    mod_board.players.at(pos) = mod_board.players.at(pos - 1);
                    mod_board.players.at(pos - 1) = helper;

                    pos--;
                } else {
                    break;
                }
            }
        });
    }
}

uint64_t clashdomeldb::finder(vector<player_s> players, eosio::name username)
{
    for (uint64_t i = 0; i < players.size(); i++)
    {
        if (players.at(i).account == username)
        {
            return i;
        }
    }
    return -1;
}

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action)
{
  if (code == receiver)
  {
    switch (action)
    {
      EOSIO_DISPATCH_HELPER(clashdomeldb, (createlboard)(resetlboard)(removelboard)(updatelboard))
    }
  }
  else
  {
    printf("Couldn't find an action.");
    return;
  }
}