#include <astatechanel.hpp>
#define TOKEN_SYMBOL symbol("STC", 4)


void astatechanel::generate(public_key address){
  users_table _users(get_self(), get_self().value);
  _users.emplace(get_self(), [&](auto& row) {
    row.id =  _users.available_primary_key();
    row.address = address;
    row.nonce = 0;
    row.timestamp = time_point_sec(current_time_point()); 
    row.balance = asset(0, TOKEN_SYMBOL);
  });  
}
void astatechanel::entangle(uint64_t id1, uint64_t id2,signature user1_sig, signature user2_sig){
  users_table _users(get_self(), get_self().value);
  auto user_itr1 = _users.find(id1);
  auto user_itr2 = _users.find(id2);
  check(user_itr1->pair == 0 , "user1 already paired");
  check(user_itr2->pair == 0 , "user2 already paired");
  check((user_itr1->balance + user_itr2->balance )!= asset(0, TOKEN_SYMBOL), "Can not open a chanel with 0 balance");
  
  uint8_t version = 0x02;
  uint8_t length = 132;
  // construct raw transaction
  uint8_t rawtx[length];
  rawtx[0] = version;
  rawtx[1] = length;
 
  memcpy(rawtx + 2  , (uint8_t *)&user_itr1->id, 8);
  memcpy(rawtx + 10 , (uint8_t *)&user_itr2->id, 8);
  memcpy(rawtx + 18 , user_itr1->address.data.data(), 33);
  memcpy(rawtx + 51 , user_itr2->address.data.data(), 33);
  memcpy(rawtx + 84 , (uint8_t *)&user_itr1->nonce, 8);
  memcpy(rawtx + 92 , (uint8_t *)&user_itr2->nonce, 8);
  memcpy(rawtx + 100, (uint8_t *)&user_itr1->timestamp, 8);
  memcpy(rawtx + 108, (uint8_t *)&user_itr2->timestamp, 8);
  memcpy(rawtx + 116, (uint8_t *)&user_itr1->balance.amount, 8);
  memcpy(rawtx + 124, (uint8_t *)&user_itr2->balance.amount, 8);

  checksum256 digest = sha256((const char *)rawtx, length);
  assert_recover_key(digest,user1_sig,user_itr1->address);
  assert_recover_key(digest,user2_sig,user_itr2->address);
  _users.modify(user_itr1, get_self(), [&](auto &row) {
    row.pair = user_itr2->id;
    row.nonce += 1;
    row.timestamp = time_point_sec(current_time_point());
  });
  _users.modify(user_itr2, get_self(), [&](auto &row) {
    row.pair = user_itr1->id;
    row.nonce += 1;
    row.timestamp = time_point_sec(current_time_point());
  });
}

void astatechanel::untangle(uint64_t id1, uint64_t id2,signature user1_sig, signature user2_sig){}
void astatechanel::transfer(uint64_t from_id, uint64_t to_id, bool from_user1, bool to_user1, asset amount, signature sig){
}
 /**
   * 提现
   */
void astatechanel::withdraw(name account, asset quantity) {
  require_auth(account);
  users_table _users(get_self(), get_self().value);
  auto user_itr = _users.find(account.value);
  check(quantity.amount > 0, "withdraw amount must be positive.");
  check(user_itr->balance >= quantity, "overdrawn balance");
  action{
    permission_level{get_self(), "active"_n},
    "token4chanel"_n,
    "transfer"_n,
    std::make_tuple(get_self(), account, quantity, std::string("Party! Your hodl is free."))
  }.send();
  _users.modify(user_itr, account, [&](auto& user){
    user.balance -= quantity;
  });
}

  /**
   * 充值
   */

void astatechanel::deposit(name from, name to, asset quantity, std::string memo) {
  if (from == get_self() || to != get_self()) {
    return;
  }
  check(quantity.amount > 0, "When pigs fly");
  check(quantity.symbol == TOKEN_SYMBOL, "These are not the droids you are looking for.");
  uint64_t id = strtoull (memo.c_str(), NULL, 0);
  users_table _users(get_self(), get_self().value);
  auto user_itr = _users.find(id);
  check(user_itr != _users.end(), "user with given id do not exist");
  check(user_itr->pair == 0, "user has been paired");
  _users.modify(user_itr, get_self(), [&](auto &row) {
    row.balance += quantity;
  });   
}

void astatechanel::clear() {
  require_auth(get_self());
  users_table _users(get_self(), get_self().value);
  for (auto _table_itr = _users.begin(); _table_itr != _users.end();) {
    _table_itr = _users.erase(_table_itr);
  }
  // Delete all records in _messages table
  auto user_itr = _users.begin();
  while (user_itr != _users.end()) {
    user_itr = _users.erase(user_itr);
  }
}


