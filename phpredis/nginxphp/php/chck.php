<?php
   $sname="auth-redis";
   $redis = new Redis();
   $redis->connect($sname, 6379);
   if($redis->auth("password")!=1){
       http_response_code(403);
   }

   if($redis->exists("ASDF")){
       $array=$redis->hgetall("ASDF");
       $jarray=json_decode($redis->get("QWER"));
       #var_dump($jarray);
       echo $jarray->{"Name"}." : ".$jarray->{"ID"};
       echo "<br>";
       echo $array["Name"]." : ".$array["ID"];
       echo "<br>";
       echo  $redis->ttl("ASDF");
   }else{
       echo "go sasa.php again";
   }
?>