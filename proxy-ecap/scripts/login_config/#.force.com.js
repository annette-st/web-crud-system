return await new Promise((resolve, reject) => {
    fetch("https://um5.lightning.force.com/one/one.app")
        .then(resp => resp.text())
        .then(data => {
            let startBracket
            let finishBracket
            let curIndex = data.indexOf('Email')
            if (curIndex !== -1) {
                let symb = data.slice(curIndex, curIndex + 1)
                while (symb !== "{") {
                    symb = data.slice(curIndex, curIndex + 1)
                    curIndex--
                }
                startBracket = curIndex + 1
                finishBracket = startBracket
                while (finishBracket !== "}") {
                    finishBracket = data.slice(curIndex, curIndex + 1)
                    curIndex++
                }
                finishBracket = curIndex
                let finalObject = data.slice(startBracket, finishBracket)
                let finalEmail = JSON.parse(finalObject)["Email"]
                console.log(finalEmail)
                resolve(finalEmail)
            }

        })
})
