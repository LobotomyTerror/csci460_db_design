async function getData() {
    const url = "getData";
    let dbData = {};
    let returnValue = '';
    try {
        const response = await fetch(url);
        if (!response.ok) {
            throw new Error(`Response status: ${response.status}`);
        }
        const json = await response.text();
        // console.log(json);
        dbData = JSON.parse(json);
        // console.log(dbData);
        dbData.forEach((data) => {
            returnValue += `<tr><td>${data.id}</td><td>${data.first}</td><td>${data.last}</td><td>${data.salary}</td></tr>`;
        });
        return returnValue;
    } catch (error) {
        console.error(error.error);
    }
}    

async function doLoad() {
    let tag = document.getElementById('salaryTable');
    if (tag) {
        data = await getData();
        // console.log(data);
        tag.innerHTML += data;
    }
}

function doDelete() {

}

let tag = document.getElementById('delete');
if (tag) {
    tag.addEventListener('click', doDelete)
}
window.addEventListener('load', doLoad);