FROM gcc:latest as builder

WORKDIR /app

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils\
    libjson-c-dev \
    python3 \
    python3-pip \
    python3-venv \
    && rm -rf /var/lib/apt/lists/*
# Copy the source code and test files
COPY . .

# Compile the application
RUN make

# Create and activate virtual environment, install pytest
RUN python3 -m venv venv
ENV PATH="/app/venv/bin:$PATH"
RUN pip install pytest

# Test stage
FROM builder as tester
CMD ["sh", "-c", "make test || (echo 'Tests failed. Server logs:' && cat /var/log/syslog && exit 1)"]

# Final stage
FROM builder as runner
# Expose the port the app runs on
EXPOSE 45234

# Run the app
CMD ["./mini-redis"]