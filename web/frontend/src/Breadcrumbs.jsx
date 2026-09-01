import { Link } from "react-router-dom";

// Renders a breadcrumb trail, e.g. Runs / b5VwLrP5oZ / Game 3. The last item
// is the current page and is rendered as plain text.
export default function Breadcrumbs({ items }) {
  return (
    <nav className="crumbs">
      {items.map((item, index) => (
        <span key={index}>
          {index > 0 && <span className="sep">/</span>}
          {item.to ? <Link to={item.to}>{item.label}</Link>
            : <span className="current">{item.label}</span>}
        </span>
      ))}
    </nav>
  );
}
