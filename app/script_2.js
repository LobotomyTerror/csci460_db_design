/* This is Front-End Code */
/* Code run in the browser */

function getData(tag) {
  fetch("/getData").then((response)=>{
    response.json().then((obj)=>{
        obj.forEach((d)=>{
          tag.innerHTML+=`<tr><td>${d.id}</td><td>${d.first}</td><td>${d.last}</td><td>${d.salary}</td></tr>`;
      })
    })
  })
}

function doLoad() {
    let tag=document.getElementById('salaryTable');
    if (tag) getData(tag);
}

window.addEventListener('load',doLoad);

