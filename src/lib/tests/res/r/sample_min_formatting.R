x<-1;y<-2.5;label<-"Alice";flag<-FALSE
nums<-c(1L,2L,3L,4L,5L);mat<-matrix(1:6,nrow=2L);100->rhs_val;extra="extra"
print(x);cat("Label:",label,"\n");message("Debug info")
square<-function(n){n*n}
add<-function(a,b){result<-a+b;return(result)}
greet<-function(nm,prefix="Hi"){msg<-paste(prefix,nm);cat(msg,"\n")}
is_positive<-function(n){if(n>0){return(TRUE)};return(FALSE)}
if(x>0){cat("positive\n")}
if(flag){cat("true\n")}else{cat("false\n")}
if(x<0){cat("neg\n")}else if(x==0){cat("zero\n")}else{cat("pos\n")}
status<-if(x>0)"high"else"low"
for(i in 1:5L){cat(i,"\n")}
for(val in nums){if(val>3L){cat("big:",val,"\n")}}
for(i in 1:3L){for(j in 1:3L){cat(i*j," ")};cat("\n")}
k<-1L;while(k<=5L){cat(k,"\n");k<-k+1L}
count<-0L;while(TRUE){count<-count+1L;if(count>=3L){break}}
rep_count<-0L;repeat{rep_count<-rep_count+1L;if(rep_count>=5L){break}}
for(i in 1:10L){if(i%%2L==0L){next};cat(i,"\n")}
squares<-sapply(1:5,function(n)n^2)
doubled_list<-lapply(nums,function(n){n*2L})
sums<-vapply(1:3,function(i)sum(1:i),numeric(1))
greeting<-paste("Hello","World");upper_label<-toupper(label)
formatted<-sprintf("x = %.2f",x);nchar(greeting)
filtered<-nums[nums>2L];nums[1]<-10L;total<-sum(nums);avg<-mean(nums)
doubled_v<-nums*2L;sorted_v<-sort(nums,decreasing=TRUE)
pipe_result<-1:10|>sum();pipe2<-nums|>rev()|>cumsum()
safe_log<-tryCatch({log(10)},error=function(e){message(conditionMessage(e));return(NA_real_)})
withCallingHandlers({sqrt(4)},warning=function(w){message("Warning:",conditionMessage(w));invokeRestart("muffleWarning")})
env<-new.env();env$value<-42L;assign("g_val",100L);rm("g_val")
make_counter<-function(){count<-0L;list(increment=function(){count<<-count+1L},get=function()count)}
counter<-make_counter();counter$increment();val<-counter$get()
!flag;-x;is.numeric(x);Sys.time();invisible(NULL)
