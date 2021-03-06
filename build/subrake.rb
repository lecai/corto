require "#{ENV['CORTO_BUILD']}/common"

COMPONENTS = [] if not defined? COMPONENTS
COMPONENTS_DONE = []

def forward(dir, task)
  if not COMPONENTS_DONE.include? dir then
    if File.exists? "#{dir}/project.json" then
      begin
        sh "corto rakefile #{dir}"
      rescue
        STDERR.puts "#{C_WARNING}warning: failed to generate rakefile for '#{dir}'#{C_NORMAL}"
      end
    end
    if File.exists? "#{dir}/rakefile" then
      sh "rake #{task} -f #{dir}/rakefile"
      COMPONENTS_DONE << dir
    end
  end
end

task :all => :default

task :default do
  verbose(VERBOSE)
  COMPONENTS.each do |e|
    forward(e, "default")
  end
  COMPONENTS_DONE.clear
end

task :collect do
  verbose(VERBOSE)
  COMPONENTS.each do |e|
    forward(e, "collect")
  end
  COMPONENTS_DONE.clear
end

task :clean do
  # Prevent cleaning twice
  if not ARGV.include? "clobber" then
    verbose(VERBOSE)
    COMPONENTS.reverse_each do |e|
      forward(e, "clean")
    end
    forward("test", "clean")
  end
end

task :clobber do
  verbose(VERBOSE)
  COMPONENTS.reverse_each do |e|
    forward(e, "clobber")
  end
  forward("test", "clobber")
  COMPONENTS_DONE.clear
end

task :test do
  if not defined? TEST
    verbose(VERBOSE)
    error = 0
    COMPONENTS.each do |e|
      if File.exists? "#{e}" then
        begin
          forward("#{Dir.pwd}/#{e}", "test")
        rescue
          error = 1
        end
      end
    end
    if File.exists? "test" then
      begin
        forward("#{Dir.pwd}/test", "runtest")
      rescue
        error = 1
      end
    end
    if error != 0 then
      abort
    end
  end
  COMPONENTS_DONE.clear
end

task :runtest do
  if not defined? TEST
    verbose(VERBOSE)
    error = 0
    COMPONENTS.each do |e|
      if File.exists? e then
        begin
          forward("#{Dir.pwd}/#{e}", "runtest")
        rescue
          error = 1
        end
      end
    end
    if error != 0 then
      abort
    end
  end
  COMPONENTS_DONE.clear
end

task :gcov do
    COMPONENTS.each do |e|
        verbose(VERBOSE)
        sh "rake gcov -f #{Dir.pwd}/#{e}/rakefile"
    end
    COMPONENTS_DONE.clear
end
