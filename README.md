# loniworkshop2018
7th LONI Parallel Programming Workshop

[Workshop Survey Link](https://goo.gl/forms/e0v5D7n05wFpAZog2)

# Log onto SuperMike2

## 1. From your *local* terminal:
<pre>
$ ssh -X hpctrn??@mike.hpc.lsu.edu <b># replace ?? with your training account number</b>
hpctrn??@mike.hpc.lsu.edu's password: <b># type in the long password, the input will NOT echo back</b>
</pre>

## 2. Start an interactive job so that you are on a compute node either in the shelob queue or training queue, remember to pass the "-X" parameter to the qsub command to enable X11 forwarding (we will use plotting in day 1 morning):
If your training account number is less than or equal to 32 (e.g. `hpctrn01 ~ hpctrn32`), submit your job to the shelob queue:
<pre>
[hpctrn??@mike1 ~]$ qsub -I <b>-X</b> -l nodes=1:ppn=16,walltime=8:00:00 <b>-q shelob</b> -A hpc_train_2018
...
</pre>
If your training account number is larger than 32 (e.g. `hpctrn33 ~ hpctrn60`), submit your job to the training queue:
<pre>
[hpctrn??@mike1 ~]$ qsub -I <b>-X</b> -l nodes=1:ppn=16,walltime=8:00:00 <b>-q training</b> -A hpc_train_2018
...
</pre>

## 3. Make sure you are on a compute node with the following command prompt, it should be mike/shelob hostname followed by a three digit number like the one below:
<pre>
[hpctrn??@mike001 ~]$ 
</pre>
OR
<pre>
[hpctrn??@shelob001 ~]$ 
</pre>

## 4. Clone the workshop repository to your home directory:
<pre>
[hpctrn??@mike001 ~]$ git clone https://github.com/lsuhpchelp/loniworkshop2018
</pre>
