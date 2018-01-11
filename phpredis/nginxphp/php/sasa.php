<?php
   $sname="auth-redis";
   $redis = new Redis();
   $redis->connect($sname, 6379);
   if($redis->auth("password")!=1){
       http_response_code(403);
   }
   $array = array(
    "Name" => "John",
    "ID" => "123456789",
   );
   $redis->hmset("ASDF",$array);
   $redis->set("QWER",'{"Name":"Joe","ID":"987654321"}');
   $redis->expire("ASDF","30");
   echo "Save data!!"
?>
