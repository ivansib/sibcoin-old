## example start
##
## python addeditdex-test.py ./sibcoin-cli -testnet


import sys
import subprocess
import copy

correct = []
correct.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"test offer details\"}")

correct.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"" + "a" * 140  + "\"" + \
    ",\"details\": \"test offer details\"}")

correct.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"" + "ф" * 140  + "\"" + \
    ",\"details\": \"test offer details\"}")

correct.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"" + "a" * 768  + "\"}")

correct.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"" + "ф" * 768  + "\"}")

correct.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"" + "す" * 768  + "\"}")

incorrect = []
incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.0000000005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.0089899999\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 11" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RA\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RAB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sull\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 10" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"test offer v3\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"" + "x" * 150  + "\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"" + "ф" * 150  + "\"" + \
    ",\"details\": \"test offer details\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"" + "short info\"" + \
    ",\"details\": \"" + "a" * 769  + "\"}")

incorrect.append("{\"type\": \"sell\"" + \
    ",\"countryIso\": \"RU\"" + \
    ",\"currencyIso\": \"RUB\"" + \
    ",\"paymentMethod\": 1" + \
    ",\"price\": \"100.005\"" + \
    ",\"minAmount\": \"10.00\"" + \
    ",\"timeToExpiration\": 10" + \
    ",\"shortInfo\": \"" + "short info\"" + \
    ",\"details\": \"" + "ф" * 769  + "\"}")

def add(pathFile, otherCommand):
    listCmd = []
    listCmd.append(pathFile)
    
    if len(otherCommand) > 0:
        listCmd.extend(otherCommand)
    
    listCmd.append("adddexoffer")

    for i, item in enumerate(correct):    
        cmd = copy.copy(listCmd)
        cmd.append(item)
        answer = subprocess.check_output(cmd).decode("utf-8")
        if answer.find("error") != -1:
            print("Wrong answer for correct query in: add", i)

    for i, item in enumerate(incorrect):
        cmd = copy.copy(listCmd)
        cmd.append(item)
        try:
            subprocess.check_output(cmd, stderr=subprocess.STDOUT).decode("utf-8")
        except subprocess.CalledProcessError as e:
            pass
        else:
            print("Wrong answer for incorrect query in: add", i)

def edit(pathFile, otherCommand):
    listCmd = []
    listCmd.append(pathFile)
    
    if len(otherCommand) > 0:
        listCmd.extend(otherCommand)
    
    cmd = copy.copy(listCmd)
    cmd.append("adddexoffer")
    cmd.append(correct[0])
    
    answer = subprocess.check_output(cmd).decode("utf-8")
    if answer.find("error") == -1:
        answer = answer.replace("{\n  \"hash\": \"", "")
        answer = answer.replace("\"\n}\n", "")

        for i, item in enumerate(incorrect):
            cmd = copy.copy(listCmd)
            cmd.append("editdexoffer")
            cmd.append(answer)
            cmd.append(item)
            try:
                subprocess.check_output(cmd, stderr=subprocess.STDOUT).decode("utf-8")
            except subprocess.CalledProcessError as e:
                pass
            else:
                print("Wrong answer for incorrect query in: edit", i)
    else:
        print("Wrong answer for correct query in edit")

def main(argv):
    pathFile = ""
    otherCommand = []
    
    if len(argv) >= 1:
        pathFile = argv[0]
        
        for a in argv[1:]:
            otherCommand.append(a)
        
        add(pathFile, otherCommand)
        edit(pathFile, otherCommand)
    else:
        print("bad arguments")

if __name__ == '__main__':
    main(sys.argv[1:])
