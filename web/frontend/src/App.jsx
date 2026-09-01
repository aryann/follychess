import { Link, Outlet } from "react-router-dom";

export default function App() {
  return (
    <>
      <header className="top">
        <div className="inner">
          <Link className="name" to="/">FollyTest</Link>
          <nav>
            <Link to="/">Runs</Link>
            <a href="https://follychess.com">FollyChess</a>
            <a href="https://github.com/aryann/follychess">GitHub</a>
          </nav>
        </div>
      </header>
      <main>
        <Outlet />
      </main>
    </>
  );
}
