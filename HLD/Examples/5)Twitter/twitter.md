Functional requirements

1) User should be able to pull his feed from his followers
2) 


Capacity estimation

1) LEt say users are 2 Billion

and Dau are 1 billion assuming 50 percent



and each has 500 friends on approx

and we assume we are generating 10 million posts per day so thats a lot of posts 

assuming latences that in the same region the messages are seen early but in some other region the time can be little more 



assuming there are twoparts to the system 

the first part is where the create post is happening and then the other part is feed generation which should be notified to as these posts are going to be seen in the feeds of the people 


so lets talk about the create post so as lots of posts are being generated in the system so we woll require some kind of load balancing so all the create posts requests go to the load balancer and then they are being routed to the api servers 

the api server task is to store in some database we will take rdbms in this case as
10 M posts per day so it is appox 112 per second and in peak conditions also *5 rdbms can handle this very nicely 

so our api servers can handle storing in the db 

also in the right part we have feed server which store feeds per user if one feed contains 1000 posts then 1000 * 10KB approx = 10 MB per user is required

so this feed servers cant store all this in memory for this scale of users so this will be in disk and the ranking server will rank them and put the feeds in the server called as feed servers so we will have to have so kind of mechanism to notify the feed servers or the ranking servers that new posts have been generated so this work will aslo be done by our api server so now this api server is doing a lot so we have a kafka queue which will  handle this so api server will just push the posts in the kafka queue and then the kafka will forward to db will store and then notify the service that new posts have been come and also other region notify can happen 