Pod::Spec.new do |s|

  s.name            = 'cpp-events'
  s.version         = '1.0.0'
  s.summary         = 'Events support for C++, using variadic templates'
  s.homepage        = 'https://github.com/cristik/cpp-events'
  s.source          = { :git => 'https://github.com/cristik/cpp-events.git', :tag => s.version.to_s }
  s.license         = { :type => 'MIT', :file => 'License.txt' }

  s.authors = {
    'Cristian Kocza'   => 'cristik@cristik.com',
  }

  s.ios.deployment_target = '5.0'
  s.osx.deployment_target = '10.7'

  s.libraries = 'c++'
  
  s.source_files = 'events.h'

end

