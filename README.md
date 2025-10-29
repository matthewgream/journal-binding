# journal-binding

Native Node.js binding for systemd journal access.

## Installation
```bash
npm install
node-gyp configure build
```

## Usage
```javascript
const Journal = require('journal-binding');

const j = new Journal();
j.addMatch('_SYSTEMD_UNIT', 'network-connector.service');
j.seekTail();

let entry;
while ((entry = j.next())) {
  console.log(entry.MESSAGE);
}

j.close();
```
