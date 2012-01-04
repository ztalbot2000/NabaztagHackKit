#! /usr/bin/env ruby
require 'stringio'


HOST   = "ssh-21560@warteschlange.de"
REMOTE = "/kunden/warteschlange.de/.tmp/OpenJabNab/bootcode"
TMP    = "tmp.mtl"
OUT    = "bootcode.bin"
FILTER = "| grep -v 'bytes' | grep -e'[a-z]'"

def rcompile(file)
  File.open(TMP, 'w') do |out|
    _merge(File.open(file), File.dirname(file), out)
  end
  `scp #{TMP} #{HOST}:#{REMOTE}/#{TMP}`
  `rm #{TMP}`
  puts `ssh #{HOST} "cd #{REMOTE} && rm -f #{OUT} && compiler/mtl_linux/mtl_comp -s #{TMP} #{OUT} 2>&1 #{FILTER}"`
  `scp #{HOST}:#{REMOTE}/#{OUT} #{OUT}`
end

def _merge(io, dir = ".", ch = STDOUT)
  io.each_line do |line|
    if line =~ /\/\/include "([^"]+)"/
      line = `cat #{dir}/#{$1}.mtl`
    end
    ch.puts line
  end
end

def merge(file)
  _merge(File.open(file), File.dirname(file))
end

def _split(io)
 [].tap do |blocks|
   current = ""
   io.each_line do |line|
     current << line
     if line.include?(";;")
       blocks << current
       current = ""
     end
   end
  end
end

def split(file)
  _split(File.open(file))
end


def mcompile(file)
  File.open("tmp.mtl", 'w') do |out|
    _merge(File.open(file), File.dirname(file), out)
  end
  puts compile("tmp.mtl")
end

def compile(file, filter = false)
  filter_cmd = filter ? "| grep done" : ""
  `../../.tmp/OpenJabNab/bootcode/compiler/mtl_linux/mtl_comp #{file} tmp.bin 2>&1 #{FILTER} #{filter_cmd}`
end

def compiles?(lines)
  write(lines, "tmp.mtl")
  compile("tmp.mtl", true).include?("done !")
end

def write(lines, file)
  File.open(file, 'w') do |f2|
    f2.puts lines.join
  end
end

def optimize(file)
  StringIO.new.tap do |out|
    _merge(File.open(file), File.dirname(file), out)
    out.pos = 0

    lines = _split(out)
    lines.size.times do |i|
      removed = lines[i]
      lines[i] = "// MISSING \n"
      if compiles?(lines)
          lines[i] = "\n/*\n#{removed}\n*/\n"
      else
        lines[i] = removed
      end
    end
    write(lines, "optimized.mtl")
  end
end


self.send(*ARGV)