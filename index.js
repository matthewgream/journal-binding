const binding = require('./build/Release/journal');

class Journal {
  constructor() {
    this.reader = new binding.JournalReader();
    this.reader.open();
  }
  
  addMatch(field, value) {
    return this.reader.addMatch(`${field}=${value}`);
  }
  
  seekTail() {
    return this.reader.seekTail();
  }
  
  next() {
    return this.reader.next();
  }
  
  close() {
    return this.reader.close();
  }
}

module.exports = Journal;
