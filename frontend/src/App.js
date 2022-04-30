import "./App.css";
import { useState } from "react"
import axios from "axios"

const API_BASE = "http://localhost:8080"

function App() {
  const [response, setResponse] = useState(null)

  const fetchResults = (keyword) => {
    axios.get(API_BASE + "/query", {
      params: { "q": keyword }
    }).then((res) => {
      console.log(res)
      setResponse(res.data)
    }).catch(console.error)
  }

  return (
    <div className="App">
      <div className="tip">Source code on <a href="https://github.com/aapeliv/goggle">GitHub</a>.</div>
      <img src="/goggle_text.png" height="200" /><br />
      <input
        type="text"
        placeholder="Search 6,487,373 Wikipedia articles"
        className="box"
        onKeyDown={(event) => {
          if (event.key === "Enter"){
            console.log("Enter key pressed")
            fetchResults(event.target.value)
          }
        }}
      /><br />
      <div className="result-box">
      {response ? (
        <>
          <div className="hint">Results returned in {response["duration_ms"]/1000} s.</div>
          {response["results"].map(res => (
            <div className="result">
              <p className="page-title"><a href={res["url"]}>{res["title"]}</a></p>
              <p className="page-snippet">...<span dangerouslySetInnerHTML={{__html: res["snippet"]}} />... <a href={res["url"]}>Read more...</a></p>
            </div>))}
        </>)
          :
        (<div className="hint">Type something and press enter to search.</div>)
      }
      </div>
    </div>
  );
}

export default App;
