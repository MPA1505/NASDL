import React, { useState } from 'react';

function App() {
  const [isLoggedIn, setIsLoggedIn] = useState(false);

  const handleLogin = (e) => {
    e.preventDefault();
    // Placeholder for authentication logic
    setIsLoggedIn(true);
  };

  const handleUnlock = () => {
    // Placeholder for sending the unlock command
  };

  return (
    <div className="App">
      {isLoggedIn ? (
        <div>
          <h2>Unlock Door</h2>
          <button onClick={handleUnlock}>Unlock</button>
        </div>
      ) : (
        <form onSubmit={handleLogin}>
          <h2>Login</h2>
          <input type="text" placeholder="Username" required />
          <input type="password" placeholder="Password" required />
          <button type="submit">Login</button>
        </form>
      )}
    </div>
  );
}

export default App;
