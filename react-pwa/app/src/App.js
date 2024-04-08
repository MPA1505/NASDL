import React, { useState } from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';

function App() {
  const [isLoggedIn, setIsLoggedIn] = useState(false);
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');

  const handleLogin = async (e) => {
    e.preventDefault();

    try {
      const response = await fetch(`${process.env.REACT_APP_BACKEND_URL}/login`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ username, password }),
      });

      if (!response.ok) {
        throw new Error('Login failed');
      }

      const data = await response.json();
      localStorage.setItem('token', data.access_token); // Store the token
      setIsLoggedIn(true);
    } catch (error) {
      alert(error.message);
    }
  };

  const handleUnlock = () => {
    // Placeholder for sending the unlock command
    alert("Unlocking door...");
  };

  return (
    <div className="container p-5">
      <div className="row justify-content-center">
        <div className="col-md-6">
          {isLoggedIn ? (
            <div className="text-center mt-5">
              <h2>Unlock door</h2>
              <button onClick={handleUnlock} className="btn btn-primary mt-3">Unlock Door</button>
            </div>
          ) : (
            <div className="card p-4 mt-5">
              <form onSubmit={handleLogin}>
                <h2 className="text-center">Login</h2>
                <div className="mb-3">
                  <label htmlFor="username" className="form-label">Username</label>
                  <input 
                    type="text" 
                    className="form-control" 
                    id="username" 
                    value={username}
                    onChange={e => setUsername(e.target.value)}
                    placeholder="Username" 
                    required />
                </div>
                <div className="mb-3">
                  <label htmlFor="password" className="form-label">Password</label>
                  <input 
                    type="password" 
                    className="form-control" 
                    id="password" 
                    value={password}
                    onChange={e => setPassword(e.target.value)}
                    placeholder="Password" 
                    required />
                </div>
                <div className="text-center">
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

