#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/system.hpp>
using namespace std;
using namespace eosio;

CONTRACT astatechanel : public contract {
  public:
    using contract::contract;
    [[eosio::action]]
    void generate(public_key address);
    [[eosio::action]]
    void entangle(uint64_t id1, uint64_t id2,signature user1_sig, signature user2_sig);
    [[eosio::action]]
    void untangle(uint64_t id1, uint64_t id2,signature user1_sig, signature user2_sig);
    [[eosio::action]]
    void transfer(uint64_t from_id, uint64_t to_id, bool from_user1, bool to_user1, asset amount, signature sig);
    [[eosio::action]]
    void clear();
    [[eosio::action]]
    void withdraw(name account, asset quantity);
    [[eosio::on_notify("token4chanel::transfer")]] 
    void deposit(name from, name to, asset quantity, std::string memo);

  private:
    struct [[eosio::table]] users {
      uint64_t        id;
      uint64_t        pair;
      public_key      address;
      uint64_t        nonce;
      time_point_sec  timestamp; 
      asset           balance;
      uint64_t primary_key()const { return id; }
    };
    typedef multi_index<name("users"), users> users_table;
};
