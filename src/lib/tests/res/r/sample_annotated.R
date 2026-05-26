# Golden sample: Contains R language features and constructs

# ============================================================================
# Assignments
# ============================================================================

x <- 1 # +1 (binary operator)
y <- 2.5 # +1 (binary operator)
label <- "Alice" # +1 (binary operator)
flag <- FALSE # +1 (binary operator)
nums <- c(1L, 2L, 3L, 4L, 5L) # +1 (binary operator)
mat <- matrix(1:6, nrow = 2L) # +1 (binary operator)
100 -> rhs_val # +1 (binary operator)
extra = "extra" # +1 (binary operator)

# ============================================================================
# Function calls
# ============================================================================

print(x) # +1 (call)
cat("Label:", label, "\n") # +1 (call)
message("Debug info") # +1 (call)

# ============================================================================
# Function definitions
# ============================================================================

square <- function(n) { # +1 (binary operator)
    n * n # +1 (binary operator)
}

add <- function(a, b) { # +1 (binary operator)
    result <- a + b # +1 (binary operator)
    return(result) # +1 (call)
}

greet <- function(nm, prefix = "Hi") { # +1 (binary operator)
    msg <- paste(prefix, nm) # +1 (binary operator)
    cat(msg, "\n") # +1 (call)
}

is_positive <- function(n) { # +1 (binary operator)
    if (n > 0) { # +1 (if statement)
        return(TRUE) # +1 (call)
    }
    return(FALSE) # +1 (call)
}

# ============================================================================
# If statements
# ============================================================================

if (x > 0) { # +1 (if statement)
    cat("positive\n") # +1 (call)
}

if (flag) { # +1 (if statement)
    cat("true\n") # +1 (call)
} else {
    cat("false\n") # +1 (call)
}

if (x < 0) { # +1 (if statement)
    cat("neg\n") # +1 (call)
} else if (x == 0) {
    cat("zero\n") # +1 (call)
} else {
    cat("pos\n") # +1 (call)
}

status <- if (x > 0) "high" else "low" # +1 (binary operator)

# ============================================================================
# For loops
# ============================================================================

for (i in 1:5L) { # +1 (for statement)
    cat(i, "\n") # +1 (call)
}

for (val in nums) { # +1 (for statement)
    if (val > 3L) { # +1 (if statement)
        cat("big:", val, "\n") # +1 (call)
    }
}

for (i in 1:3L) { # +1 (for statement)
    for (j in 1:3L) { # +1 (for statement)
        cat(i * j, " ") # +1 (call)
    }
    cat("\n") # +1 (call)
}

# ============================================================================
# While loops
# ============================================================================

k <- 1L # +1 (binary operator)
while (k <= 5L) { # +1 (while statement)
    cat(k, "\n") # +1 (call)
    k <- k + 1L # +1 (binary operator)
}

count <- 0L # +1 (binary operator)
while (TRUE) { # +1 (while statement)
    count <- count + 1L # +1 (binary operator)
    if (count >= 3L) { # +1 (if statement)
        break # +1 (break)
    }
}

# ============================================================================
# Repeat loops
# ============================================================================

rep_count <- 0L # +1 (binary operator)
repeat { # +1 (repeat statement)
    rep_count <- rep_count + 1L # +1 (binary operator)
    if (rep_count >= 5L) { # +1 (if statement)
        break # +1 (break)
    }
}

# ============================================================================
# Next keyword
# ============================================================================

for (i in 1:10L) { # +1 (for statement)
    if (i %% 2L == 0L) { # +1 (if statement)
        next # +1 (next)
    }
    cat(i, "\n") # +1 (call)
}

# ============================================================================
# Higher-order functions
# ============================================================================

squares <- sapply(1:5, function(n) n ^ 2) # +1 (binary operator)

doubled_list <- lapply(nums, function(n) { # +1 (binary operator)
    n * 2L # +1 (binary operator)
})

sums <- vapply(1:3, function(i) sum(1:i), numeric(1)) # +1 (binary operator)

# ============================================================================
# String operations
# ============================================================================

greeting <- paste("Hello", "World") # +1 (binary operator)
upper_label <- toupper(label) # +1 (binary operator)
formatted <- sprintf("x = %.2f", x) # +1 (binary operator)
nchar(greeting) # +1 (call)

# ============================================================================
# Vector operations
# ============================================================================

filtered <- nums[nums > 2L] # +1 (binary operator)
nums[1] <- 10L # +1 (binary operator)
total <- sum(nums) # +1 (binary operator)
avg <- mean(nums) # +1 (binary operator)
doubled_v <- nums * 2L # +1 (binary operator)
sorted_v <- sort(nums, decreasing = TRUE) # +1 (binary operator)

# ============================================================================
# Pipe operator
# ============================================================================

pipe_result <- 1:10 |> sum() # +1 (binary operator)
pipe2 <- nums |> rev() |> cumsum() # +1 (binary operator)

# ============================================================================
# Error handling
# ============================================================================

safe_log <- tryCatch({ # +1 (binary operator)
    log(10) # +1 (call)
}, error = function(e) {
    message(conditionMessage(e)) # +1 (call)
    return(NA_real_) # +1 (call)
})

withCallingHandlers({ # +1 (call)
    sqrt(4) # +1 (call)
}, warning = function(w) {
    message("Warning:", conditionMessage(w)) # +1 (call)
    invokeRestart("muffleWarning") # +1 (call)
})

# ============================================================================
# Environments
# ============================================================================

env <- new.env() # +1 (binary operator)
env$value <- 42L # +1 (binary operator)
assign("g_val", 100L) # +1 (call)
rm("g_val") # +1 (call)

# ============================================================================
# Closures
# ============================================================================

make_counter <- function() { # +1 (binary operator)
    count <- 0L # +1 (binary operator)
    list( # +1 (call)
        increment = function() {
            count <<- count + 1L # +1 (binary operator)
        },
        get = function() count
    )
}

counter <- make_counter() # +1 (binary operator)
counter$increment() # +1 (call)
val <- counter$get() # +1 (binary operator)

# ============================================================================
# Unary operators and misc
# ============================================================================

!flag # +1 (unary operator)
-x # +1 (unary operator)
is.numeric(x) # +1 (call)
Sys.time() # +1 (call)
invisible(NULL) # +1 (call)
