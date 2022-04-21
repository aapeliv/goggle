import './App.css';
import DATA from './MOCK_DATA.json'
import {useState} from 'react'


function App() {
  const [searchTerm, setSearchTerm] = useState("")
  return (
    <div className="App">
      <input 
      type = "text" 
      placeholder = "Search String" 
      onChange = {(event) => {
        setSearchTerm(event.target.value);
      }}
      />
      {DATA.filter((val) => {
        if (val.first_name.toLowerCase().includes(searchTerm.toLowerCase())){
          return val;
        }
      }).map((val, key) => {
        return (
          <div classname = "document" key = {key}>
            <p>{val.first_name}</p>
          </div>
        );
      })}
    </div>
  );
}

export default App;
