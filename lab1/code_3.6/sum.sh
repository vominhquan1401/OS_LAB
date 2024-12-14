#!/bin/bash

ANS_FILE="ans.txt"
HIST_FILE="history.txt"

if [ ! -f $ANS_FILE ]; then
    echo "0" > $ANS_FILE
fi

get_ans() {
    ANS=$(cat $ANS_FILE)
}

save_ans() {
    echo "$1" > $ANS_FILE
}

add_to_history() {
    echo "$1" >> $HIST_FILE
    tail -n 5 $HIST_FILE > temp.txt && mv temp.txt $HIST_FILE
}

show_history() {
    if [ -f $HIST_FILE ]; then
        cat $HIST_FILE
    else
        echo "No history available."
    fi
}

get_ans


while true; do
    echo -n ">> "
    read input

    if [ "$input" == "HIST" ]; then
        show_history
        continue
    elif [ "$input" == "EXIT" ]; then
        echo "EXIT"
        break
    fi


    input=$(echo $input | sed "s/ANS/$ANS/g")


    if [[ $input =~ /[[:space:]]*0$ ]]; then
        echo "MATH ERROR"
    else

        result=$(echo "scale=2; $input" | bc 2>/dev/null)

        if [[ -z "$result" ]] || [[ "$result" == "MATH ERROR" ]]; then
            echo "SYNTAX ERROR"
        else
            echo "$result"
            save_ans $result
            add_to_history "$input = $result"
            ANS=$result
        fi
    fi

    echo -n "Press any key to continue..."
    read -n 1 -s
    clear
done

