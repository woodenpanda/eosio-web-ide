# eosio-cpp contract/talk.cpp
cleos create account eosio talk EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set code talk talk.wasm
cleos set abi talk talk.abi
cleos create account eosio bob EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos push action talk post '[1000, 0, bob, "This is a new post"]' -p bob
cleos push action talk post '[2000, 0, jane, "This is my first post"]' -p jane
cleos push action talk post '[1001, 2000, bob, "Replying to your post"]' -p bob

# eosio-cpp -abigen -I ./token4chanel/include -R ./resource -contract token4chanel -o ./token4chanel/token4chanel.wasm token4chanel/src/token4chanel.cpp

cleos set contract token4chanel token4chanel -p token4chanel@active

# eosio-cpp -abigen -I ./astatechanel/include -R ./resource -contract astatechanel -o ./astatechanel/astatechanel.wasm astatechanel/src/astatechanel.cpp
cleos create account eosio astatechanel EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set contract astatechanel astatechanel -p astatechanel@active