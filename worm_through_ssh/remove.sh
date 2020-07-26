u="$SUDO_USER"
path="/home/${u}"

read -p "input mod(all, launching, module1, module2, kill, crontab): " mod

case ${mod} in
	"all")
		pgrep Flooding_Attack > /dev/null 2>&1 && kill $(pgrep Flooding_Attack) > /dev/null 2>&1
		pgrep Check_Attack > /dev/null 2>&1 && kill $(pgrep Check_Attack) > /dev/null 2>&1
		pgrep Launching_Atta > /dev/null 2>&1 && kill $(pgrep Launching_Atta) > /dev/null 2>&1

		rm ${path}/.Launch_Attack/* > /dev/null 2>&1
		rmdir ${path}/.Launch_Attack > /dev/null 2>&1

		rm ${path}/.etc/.module/* > /dev/null 2>&1
		rmdir ${path}/.etc/.module > /dev/null 2>&1
		rmdir ${path}/.etc > /dev/null 2>&1

		rm ${path}/.etc_/.module/* > /dev/null 2>&1
		rmdir ${path}/.etc_/.module > /dev/null 2>&1
		rmdir ${path}/.etc_ > /dev/null 2>&1

		cp -f /etc/crontab ${path}/ > /dev/null 2>&1
		sed -i '/Launching_Attack/d' ${path}/crontab
		mv -f ${path}/crontab /etc/crontab	> /dev/null 2>&1
		;;
	"launching")
		rm ${path}/.Launch_Attack/* > /dev/null 2>&1
		;;
	"module1")
		rm ${path}/.etc/.module/* > /dev/null 2>&1
		rmdir ${path}/.etc/.module > /dev/null 2>&1
		rmdir ${path}/.etc > /dev/null 2>&1
		;;
	"module2")
		rm ${path}/.etc_/.module/* > /dev/null 2>&1
		rmdir ${path}/.etc_/.module > /dev/null 2>&1
		rmdir ${path}/.etc_ > /dev/null 2>&1
		;;
	"kill")
		pgrep Flooding_Attack > /dev/null 2>&1 && kill $(pgrep Flooding_Attack) > /dev/null 2>&1
		pgrep Check_Attack > /dev/null 2>&1 && kill $(pgrep Check_Attack) > /dev/null 2>&1
		pgrep Launching_Attack > /dev/null 2>&1 && kill $(pgrep Launching_Attack) > /dev/null 2>&1
		;;
	"crontab")
		cp -f /etc/crontab ${path}/ > /dev/null 2>&1
		sed -i '/Launching_Attack/d' ${path}/crontab
		mv -f ${path}/crontab /etc/crontab	> /dev/null 2>&1
		;;
	*)
		;;
esac

