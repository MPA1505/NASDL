FROM node:19-alpine as build

WORKDIR /app

COPY app/package*.json ./

RUN npm install

COPY app/ .

RUN npm run build

# Serve stage
FROM nginx:stable-alpine as serve

# Copy the built app from the build stage
COPY --from=build /app/build /usr/share/nginx/html

# Expose port 80 to the outside
EXPOSE 80

# Start Nginx and keep it running in the foreground
CMD ["nginx", "-g", "daemon off;"]
