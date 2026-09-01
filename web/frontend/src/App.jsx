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
        FollyTest holds the test results for{" "}
        <a href="https://follychess.com">FollyChess</a>.
      </footer>
    </>
  );
}
