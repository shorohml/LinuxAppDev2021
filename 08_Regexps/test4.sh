test "` ./esub '(1.*7.*(8.*2))' '\2-\1=' '15 16 17 18 19 20 21' `" == "` echo '15 16 17 18 19 20 21' | sed -E 's/(1.*7.*(8.*2))/\2-\1=/' `"