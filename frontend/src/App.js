import './App.css';
import DATA from './MOCK_DATA.json'
import {useState} from 'react'
import axios from "axios"

const API_BASE = "http://localhost:8080"

function App() {
  const [searchTerm, setSearchTerm] = useState("")
  const [response, setResponse] = useState(null)

  const fetchResults = (keyword) => {
    axios.get(API_BASE + "/query", {
      params: {
        "q": keyword,
        // page length
        "pl": 10,
      }
    }).then((res) => {
      console.log(res)
      setResponse(res.data)
    }).catch(console.error)
  }

  return (
    <div className="App">
      <input 
      type = "text" 
      placeholder = "Search String" 
      // onChange = {(event) => {
      //   setSearchTerm(event.target.value);
      // }}
      onKeyDown={(event) => {
        if (event.key=== 'Enter'){
          console.log("Enter key pressed")
          // setSearchTerm(event.target.value)
          fetchResults(event.target.value)
        }
      }}
      /><br />
      {response ? (
        <>
          <small>Returned results in {response["duration_ms"]} ms</small>
          {response["results"].map(res => (
            <>
              <br />
              <b>{res["title"]}</b><br />
              <p>Rank: {res["pagerank"]}, id: {res["id"]}</p>
              <hr/>
            </>))}
        </>
      )
        :
      (<div>No results yet</div>)
      }
        <br />
        <br />
        <br />
        <br />
        <b>DEBUG:</b><br />
        <code>{JSON.stringify(response)}</code>
      {/* {DATA.filter((val) => {
        if (val.first_name.toLowerCase().includes(searchTerm.toLowerCase())){
          return val;
        }
      }).map((val, key) => {
        return (
          <div classname = "document" key = {key}>
            <p>{val.first_name}</p>
          </div>
        );
      })} */}
    </div>
  );
}

export default App;
