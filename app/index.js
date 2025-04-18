const express = require('express')
const path = require('path');
const app = express()
const port = 3000

const options = {
    root: path.join(__dirname)
}

app.get('/index.html', (req, res) => {
    res.sendFile('index.html', options);
})

app.get('/script.js', (req, res) => {
    res.sendFile('script.js', options);
})

app.get('/styles.css', (req, res) => {
    res.sendFile('styles.css', options);
})

app.get('/getData', (req, res) => {
    res.send([
        {id:2, first:"Bob", last:"Smith", salary:200000},
        {id:1, first:"Dan", last:"Fishbein", salary:100000}
    ])
})

app.listen(port, () => {
    console.log(`Example app listening on port ${port}`)
})
