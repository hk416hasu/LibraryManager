const dateStr = new Date().toISOString().split('T')[0];
let date = new Date(dateStr);
daysToAdd = 30; // TODO
date.setDate(date.getDate() + daysToAdd);
console.log(date.toISOString().split('T')[0]);
