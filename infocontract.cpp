/*
---------------------------------------------
infocontract..cpp (16.mai.2020 - Beginn)

V1.0 12.mai.2020 - Beginn S.P.

todo:
---------------------------------------------
*/


#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp> // tapos...
#include <eosiolib/crypto.h>        // sha...

#include <eosiolib/time.hpp>
//#include <eosiolib/system.hpp>
#include "infocontract.hpp"     



using namespace eosio;
using namespace std;


 
const std::string   version   = "V1.0";
 

CONTRACT infocontract : public eosio::contract {

public:
      using contract::contract;
      
      // ---
      // Constants
      //
      const uint32_t     hashvalue    = 1234567333;      
 
      
      struct param_struct
      {
      std::string param;    
      };

      

      struct transfer_args
      {
      name from;
      name to;
      asset quantity;
      std::string memo;
      };    

    
   
    

      // ---
      // struct myglobal
      //
      struct [[eosio::table]] myglobals
      {
      uint64_t id; 
      uint64_t maxdata; 
      
      auto primary_key() const { return id; }
      EOSLIB_SERIALIZE(myglobals, (id)(maxdata) ) 
      }; // struct myglobals      
      typedef eosio::multi_index< name("myglobals"), myglobals> globals;
            
            
            
      // ---
      // struct entry (for simple data-home)
      //
      struct [[eosio::table]] home
      {
      uint64_t      id; 
      std::string   dataname;
      std::string   text;
            
      auto primary_key() const { return id; }
      EOSLIB_SERIALIZE(home, (id)(dataname)(text) ) 
      }; // struct global      
      typedef eosio::multi_index< name("home"), home> entries;
            
           
     

 
    
      inline void splitMemo(std::vector<std::string> &results, std::string memo)
            {
            auto end = memo.cend();
            auto start = memo.cbegin();

            for (auto it = memo.cbegin(); it != end; ++it)
                {
                if (*it == ';')
                   {
                   results.emplace_back(start, it);
                   start = it + 1;
                   }
                }
            if (start != end)
            results.emplace_back(start, end);
            } // splitMemo


 
     
   
    
      // -------------------------------------
      // INIT - Calls THIS funktion FIRST 
      // -------------------------------------
      [[eosio::action]]  
      void init() 
      {    
      require_auth(get_self());
            
      // ---
      // Globals
      //
      int id = 0;
      globals myglobals( _self , _self.value );         
      auto iterator = myglobals.find(id);
    
      if ( iterator != myglobals.end() )
         {
         print(" globals does exist");
         } else
              {
              print(" globals will be created... ");
              myglobals.emplace(_self, [&](auto& global) 
                 {
                 global.id = id;
                 global.maxdata        = 0;
                 });
              } // else
                        
      } // init()     

      
      
      // ---
      // admin - xxx.
      //
      [[eosio::action]]  
      void newdata(  std::string dataname,  std::string text ) 
      {    
      require_auth(get_self());
              
      //
      // Get globals
      //
      int id = 0;
      globals myglobals(_self, _self.value);
      auto iterator_globals = myglobals.find(id);

      uint64_t maxdata = iterator_globals->maxdata;
      print(" maxdata(globals):" , maxdata," ");
      maxdata++;
         
      entries myentry( _self , _self.value );       
          
      myentry.emplace(_self, [&](auto& tupel) 
           {
           tupel.id         = maxdata;
           tupel.dataname   = dataname;
           tupel.text       = text;         
           });
           
  
      // ---  
      // Finaly update globals
      //
      if ( iterator_globals != myglobals.end() )
         {                        
         myglobals.modify(iterator_globals, _self, [&](auto& global) 
                  {                                                    
                  global.maxdata           = maxdata;                                                            
                  });            
         } // if...
        
        
      print(" FIN2 "); 
      } // newdata()
      
      
      
      
      [[eosio::action]]  
      void editdata(  uint64_t dataid, std::string dataname, std::string text ) 
      {   
      require_auth(get_self());
      
     
      
      entries myentry( _self , _self.value );           
      auto iterator_myentry = myentry.find( dataid );
             
      if ( iterator_myentry != myentry.end() )    
         {                   
         myentry.modify(iterator_myentry, _self, [&](auto& tupel) 
                 {                                  
                 tupel.dataname   = dataname;       
                 tupel.text   = text;     
                 });  
             
         } // modify
         else
             {
             int ret = false;
             eosio_assert( ret == true , " ERROR - index not found! ");          
             return;
             } 
              
       
      } // editdata      
      
      
      // ---
      // deldata - remove data entry
      //
      [[eosio::action]]  
      void deldata(  uint64_t id  ) 
      {
      require_auth(get_self());
      
      entries myentry( _self , _self.value );           
      auto iterator_myentry = myentry.find( id );
                  
      myentry.erase( iterator_myentry );            
      }  // deldata
      
      
      
            
      // ---
      // reset - remove globals
      //            
      [[eosio::action]]  
      void reset() 
      {   
      require_auth(get_self());
            
      globals myglobals(_self, _self.value);
         
      for (auto itr = myglobals.begin(); itr != myglobals.end();) 
          {
          itr = myglobals.erase(itr);
          }              
      } // reset()                 
            
      
      
      
 
      
      
      
}; // CONTRACT infocontract      




extern "C"
{

void apply(uint64_t receiver, uint64_t code, uint64_t action) 
{
 
   if (action == name("init").value)
       {
       execute_action(name(receiver), name(code), &infocontract::init);  
       }
    
    if (action == name("newdata").value)
       {
       execute_action(name(receiver), name(code), &infocontract::newdata);  
       }

    if (action == name("editdata").value)
       {
       execute_action(name(receiver), name(code), &infocontract::editdata);  
       }

    if (action == name("deldata").value)
       {
       execute_action(name(receiver), name(code), &infocontract::deldata);  
       }
  
    if (action == name("reset").value)
       {
       execute_action(name(receiver), name(code), &infocontract::reset);  
       }

} // apply
    
    
    
       
} // extern "C"      

