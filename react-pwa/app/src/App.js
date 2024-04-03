import React, { useState } from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';

function App() {
  const [isLoggedIn, setIsLoggedIn] = useState(false);

  const handleLogin = (e) => {
    e.preventDefault();
    // Placeholder for authentication logic
    setIsLoggedIn(true);
  };

  const handleUnlock = () => {
    // Placeholder for sending the unlock command
    alert("Unlocking door...");
  };

  return (
    <div className="container p-5"> {/* Container with padding */}
      <div className="row justify-content-center"> {/* Center content horizontally */}
        <div className="col-md-6">
          {isLoggedIn ? (
            <div className="text-center mt-5"> {/* Center text and add top margin */}
              <h2>Unlock door</h2>
              <button onClick={handleUnlock} className="btn btn-primary mt-3">Unlock Door</button>
            </div>
          ) : (
            <div className="card p-4 mt-5"> {/* Card with padding and top margin */}
              <form onSubmit={handleLogin}>
                <h2 className="text-center">Login</h2>
                <div className="mb-3"> {/* Margin bottom */}
                  <label htmlFor="username" className="form-label">Username</label>
                  <input type="text" className="form-control" id="username" placeholder="Username" required />
                </div>
                <div className="mb-3"> {/* Margin bottom */}
                  <label htmlFor="password" className="form-label">Password</label>
                  <input type="password" className="form-control" id="password" placeholder="Password" required />
                </div>
                <div className="text-center"> {/* Center button */}
                  <button type="submit" className="btn btn-success">Login</button>
                </div>
              </form>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

export default App;
