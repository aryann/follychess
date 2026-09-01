import React from "react";
import ReactDOM from "react-dom/client";
import { BrowserRouter, Route, Routes } from "react-router-dom";

import App from "./App";
import GameView from "./pages/GameView";
import RunDetail from "./pages/RunDetail";
import RunList from "./pages/RunList";
import "./style.css";

ReactDOM.createRoot(document.getElementById("root")).render(
  <React.StrictMode>
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<App />}>
          <Route index element={<RunList />} />
          <Route path="runs/:runId" element={<RunDetail />} />
          <Route path="runs/:runId/games/:gameId" element={<GameView />} />
        </Route>
      </Routes>
    </BrowserRouter>
  </React.StrictMode>,
);
