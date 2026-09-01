import { Link, Outlet } from "react-router-dom";

export default function App() {
  return (
    <>
      <header className="top">
        <div className="inner">
          <Link className="name" to="/">FollyTest</Link>
        </div>
      </header>
      <main>
        <Outlet />
      </main>
      <footer>
        FollyTest is the UI for{" "}
        <a href="https://follychess.com">FollyChess</a> Sequential
        Probability Ratio Test (SPRT) results.
      </footer>
    </>
  );
}
